#include "http_server.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sys/event.h>
#include <fcntl.h>

namespace simple_http_server {

HttpServer::HttpServer(const std::string& host, int port) : host_(host), port_(port), server_fd_(-1), kq_(-1) {}

HttpServer::~HttpServer() {
    if (server_fd_ != -1) close(server_fd_);
    if (kq_ != -1) close(kq_);
}

void HttpServer::register_handler(const std::string& uri, HttpMethod method,
                                  std::function<HttpResponse(const HttpRequest&)> handler) {
    handlers_[uri][method] = handler;
}

void HttpServer::start() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    int flags = fcntl(server_fd_, F_GETFL, 0);
    fcntl(server_fd_, F_SETFL, flags | O_NONBLOCK);

    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);
    if (bind(server_fd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind socket\n";
        close(server_fd_);
        return;
    }

    if (listen(server_fd_, MAX_CONNECTIONS) < 0) {
        std::cerr << "Failed to listen\n";
        close(server_fd_);
        return;
    }

    kq_ = kqueue();
    if (kq_ == -1) {
        std::cerr << "Failed to create kqueue\n";
        close(server_fd_);
        return;
    }

    struct kevent ev;
    EV_SET(&ev, server_fd_, EVFILT_READ, EV_ADD, 0, 0, nullptr);
    if (kevent(kq_, &ev, 1, nullptr, 0, nullptr) == -1) {
        std::cerr << "Failed to register server socket with kqueue\n";
        close(server_fd_);
        close(kq_);
        return;
    }

    std::cout << "Server listening on " << host_ << ":" << port_ << "\n";
    process_events();
}

void HttpServer::process_events() {
    struct kevent events[MAX_CONNECTIONS];
    std::unordered_map<int, std::string> client_buffers;
    std::unordered_map<int, std::string> client_responses;

    while (true) {
        int nev = kevent(kq_, nullptr, 0, events, MAX_CONNECTIONS, nullptr);
        if (nev < 0) {
            std::cerr << "kevent error\n";
            return;
        }

        for (int i = 0; i < nev; i++) {
            int fd = events[i].ident;

            if (fd == server_fd_) {
                int client_fd = accept(server_fd_, nullptr, nullptr);
                if (client_fd < 0) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK)
                        std::cerr << "Failed to accept connection\n";
                    continue;
                }

                int flags = fcntl(client_fd, F_GETFL, 0);
                fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

                struct kevent ev;
                EV_SET(&ev, client_fd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
                if (kevent(kq_, &ev, 1, nullptr, 0, nullptr) == -1) {
                    std::cerr << "Failed to register client socket\n";
                    close(client_fd);
                    continue;
                }
            } else if (events[i].filter == EVFILT_READ) {
                char buffer[4096];
                int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
                if (bytes_read < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                    close(fd);
                    client_buffers.erase(fd);
                    client_responses.erase(fd);
                    continue;
                }
                if (bytes_read == 0) {
                    close(fd);
                    client_buffers.erase(fd);
                    client_responses.erase(fd);
                    continue;
                }
                buffer[bytes_read] = '\0';
                client_buffers[fd] += buffer;

                if (client_buffers[fd].find("\r\n\r\n") != std::string::npos) {
                    HttpRequest request;
                    if (!request.parse(client_buffers[fd])) {
                        close(fd);
                        client_buffers.erase(fd);
                        client_responses.erase(fd);
                        continue;
                    }
                    request.log();

                    HttpResponse response;
                    auto uri_handlers = handlers_.find(request.uri);
                    if (uri_handlers != handlers_.end()) {
                        auto handler = uri_handlers->second.find(request.method);
                        if (handler != uri_handlers->second.end()) {
                            response = handler->second(request);
                        } else {
                            response = HttpResponse(HttpStatusCode::MethodNotAllowed);
                            response.set_content("Method Not Allowed");
                        }
                    } else {
                        response = HttpResponse(HttpStatusCode::NotFound);
                        response.set_content("Not Found");
                    }

                    // Minimal keep-alive support
                    response.set_header("Connection", "keep-alive");

                    client_responses[fd] = response.to_string();
                    client_buffers[fd].clear();

                    struct kevent ev;
                    EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, nullptr);
                    if (kevent(kq_, &ev, 1, nullptr, 0, nullptr) == -1) {
                        close(fd);
                        client_responses.erase(fd);
                    }
                }
            } else if (events[i].filter == EVFILT_WRITE) {
                if (client_responses.find(fd) == client_responses.end()) continue;
                std::string& response = client_responses[fd];
                int bytes_written = write(fd, response.c_str(), response.length());
                if (bytes_written < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                    close(fd);
                    client_responses.erase(fd);
                    client_buffers.erase(fd);
                    continue;
                }
                response.erase(0, bytes_written);
                if (response.empty()) {
                    client_responses.erase(fd);
                    struct kevent ev;
                    EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, nullptr); // Re-enable read
                    kevent(kq_, &ev, 1, nullptr, 0, nullptr);
                }
            }
        }
    }
}

} // namespace simple_http_server
