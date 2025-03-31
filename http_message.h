#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <string>
#include <unordered_map>

namespace simple_http_server {

enum class HttpMethod {
    GET,
    HEAD,
    UNKNOWN
};

enum class HttpStatusCode {
    Ok = 200,
    NotFound = 404,
    MethodNotAllowed = 405
};

class HttpRequest {
public:
    HttpMethod method;
    std::string uri;
    std::string version;
    std::unordered_map<std::string, std::string> headers;

    HttpRequest() : method(HttpMethod::UNKNOWN), uri("/"), version("HTTP/1.1") {}
    bool parse(const std::string& raw_request);
};

class HttpResponse {
public:
    HttpStatusCode status;
    std::unordered_map<std::string, std::string> headers;
    std::string content;

    HttpResponse(HttpStatusCode status = HttpStatusCode::Ok) : status(status) {}
    void set_header(const std::string& key, const std::string& value);
    void set_content(const std::string& content);
    std::string to_string() const;
};

} // namespace simple_http_server

#endif // HTTP_MESSAGE_H
