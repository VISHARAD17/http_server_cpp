#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <functional>
#include <string>
#include <unordered_map>
#include "http_message.h"

namespace simple_http_server {

class HttpServer {
public:
    HttpServer(const std::string& host, int port);
    ~HttpServer();
    void register_handler(const std::string& uri, HttpMethod method,
                         std::function<HttpResponse(const HttpRequest&)> handler);
    void start();

private:
    std::string host_;
    int port_;
    int server_fd_;
    std::unordered_map<std::string, std::unordered_map<HttpMethod, std::function<HttpResponse(const HttpRequest&)>>> handlers_;

    void handle_client(int client_fd);
};

} // namespace simple_http_server

#endif // HTTP_SERVER_H
