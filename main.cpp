#include "http_server.h"
using namespace simple_http_server;
using namespace std;

int main() {
    HttpServer server("0.0.0.0", 8080);

    // Register handlers
    auto say_hello = [](const HttpRequest&) {
        HttpResponse response(HttpStatusCode::Ok);
        response.set_header("Content-Type", "text/plain");
        response.set_content("Hello, world\n");
        return response;
    };

    auto send_html = [](const HttpRequest&) {
        HttpResponse response(HttpStatusCode::Ok);
        response.set_header("Content-Type", "text/html");
        response.set_content("<!DOCTYPE html><html><body><h1>Hello, world!</h1></body></html>");
        return response;
    };

    server.register_handler("/", HttpMethod::GET, say_hello);
    server.register_handler("/", HttpMethod::HEAD, say_hello);
    server.register_handler("/hello.html", HttpMethod::GET, send_html);
    server.register_handler("/hello.html", HttpMethod::HEAD, send_html);

    server.start();
    return 0;
}
