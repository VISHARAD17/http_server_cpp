#include "http_message.h"
#include <sstream>
using namespace std;

namespace simple_http_server {

bool HttpRequest::parse(const string& raw_request) {
    istringstream iss(raw_request);
    string line;

    // Parse request line
    if (!getline(iss, line)) return false;
    istringstream iss_line(line);
    string method_str;
    iss_line >> method_str >> uri >> version;

    if (method_str == "GET") method = HttpMethod::GET;
    else if (method_str == "HEAD") method = HttpMethod::HEAD;
    else method = HttpMethod::UNKNOWN;

    // Parse headers
    while (getline(iss, line) && line != "\r") {
        if (line.empty()) continue;
        size_t colon_pos = line.find(':');
        if (colon_pos != string::npos) {
            string key = line.substr(0, colon_pos);
            string value = line.substr(colon_pos + 1);
            while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) value.erase(0, 1);
            headers[key] = value;
        }
    }
    return true;
}

void HttpResponse::set_header(const string& key, const string& value) {
    headers[key] = value;
}

void HttpResponse::set_content(const string& content) {
    this->content = content;
    set_header("Content-Length", ::to_string(content.length()));
}

std::string HttpResponse::to_string() const {
    ostringstream oss;
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
