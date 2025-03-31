#include "http_message.h"
#include <sstream>
#include <iostream>
#include <ctime>

namespace simple_http_server {

bool HttpRequest::parse(const std::string& raw_request) {
    std::istringstream iss(raw_request);
    std::string line;

    if (!std::getline(iss, line)) return false;
    std::istringstream iss_line(line);
    std::string method_str;
    iss_line >> method_str >> uri >> version;

    if (method_str == "GET") method = HttpMethod::GET;
    else if (method_str == "HEAD") method = HttpMethod::HEAD;
    else method = HttpMethod::UNKNOWN;

    while (std::getline(iss, line) && line != "\r") {
        if (line.empty()) continue;
        std::size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);
            while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) value.erase(0, 1);
            headers[key] = value;
        }
    }
    return true;
}

void HttpRequest::log() const {
    std::time_t now = std::time(nullptr);
    char time_str[26];
    ctime_r(&now, time_str);
    time_str[24] = '\0';

    std::string method_str;
    switch (method) {
        case HttpMethod::GET: method_str = "GET"; break;
        case HttpMethod::HEAD: method_str = "HEAD"; break;
        default: method_str = "UNKNOWN"; break;
    }

    std::cout << "[" << time_str << "] " << method_str << " " << uri << " " << version << "\n";
    for (const auto& [key, value] : headers) {
        std::cout << "[" << time_str << "] Header: " << key << ": " << value << "\n";
    }
}

void HttpResponse::set_header(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void HttpResponse::set_content(const std::string& content) {
    this->content = content;
    set_header("Content-Length", std::to_string(content.length()));
}

std::string HttpResponse::to_string() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 ";
    switch (status) {
        case HttpStatusCode::Ok: oss << "200 OK"; break;
        case HttpStatusCode::NotFound: oss << "404 Not Found"; break;
        case HttpStatusCode::MethodNotAllowed: oss << "405 Method Not Allowed"; break;
        default: oss << "500 Internal Server Error";
    }
    oss << "\r\n";

    for (const auto& [key, value] : headers) {
        oss << key << ": " << value << "\r\n";
    }
    oss << "\r\n" << content;
    return oss.str();
}

} // namespace simple_http_server
