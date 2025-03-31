#include "http_server.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <thread>

namespace simple_http_server {

HttpServer::HttpServer(const std::string& host, int port) : host_(host), port_(port), server_fd_(-1) {}

HttpServer::~HttpServer() {
    if (server_fd_ != -1) close(server_fd_);
}

void HttpServer::register_handler(const std::string& uri, HttpMethod method,
                                  std::function<HttpResponse(const HttpRequest&)> handler) {
    handlers_[uri][method] = handler;
}

void HttpServer::start() {
    // Create socket
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    // Set socket options
    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind socket
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // Bind to 0.0.0.0
    addr.sin_port = htons(port_);
    if (bind(server_fd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind socket\n";
        close(server_fd_);
        return;
    }

    // Listen
    if (listen(server_fd_, 10) < 0) {
        std::cerr << "Failed to listen\n";
        close(server_fd_);
        return;
    }

    std::cout << "Server listening on " << host_ << ":" << port_ << "\n";

    // Accept connections
    while (true) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0) {
            std::cerr << "Failed to accept connection\n";
            continue;
        }
        std::thread(&HttpServer::handle_client, this, client_fd).detach();
    }
}

void HttpServer::handle_client(int client_fd) {
    char buffer[1024] = {0};
    read(client_fd, buffer, sizeof(buffer) - 1);

    HttpRequest request;
    request.parse(buffer);
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

    std::string response_str = response.to_string();
    write(client_fd, response_str.c_str(), response_str.length());
    close(client_fd);
}

} // namespace simple_http_server
