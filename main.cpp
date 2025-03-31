#include "http_server.h"
#include <iostream>

int main() {
    simple_http_server::HttpServer server("0.0.0.0", 8080);

    auto say_hello = [](const simple_http_server::HttpRequest&) {
        simple_http_server::HttpResponse response(simple_http_server::HttpStatusCode::Ok);
        response.set_header("Content-Type", "text/plain");
        response.set_content("Hello, world\n");
        return response;
    };

    auto send_html = [](const simple_http_server::HttpRequest&) {
        simple_http_server::HttpResponse response(simple_http_server::HttpStatusCode::Ok);
        response.set_header("Content-Type", "text/html");
        response.set_content("<!DOCTYPE html><html><body><h1>Hello, world!</h1></body></html>");
        return response;
    };

    server.register_handler("/", simple_http_server::HttpMethod::GET, say_hello);
    server.register_handler("/", simple_http_server::HttpMethod::HEAD, say_hello);
    server.register_handler("/hello.html", simple_http_server::HttpMethod::GET, send_html);
    server.register_handler("/hello.html", simple_http_server::HttpMethod::HEAD, send_html);

    server.start();
    return 0;
}
