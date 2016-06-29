#include "http_response.hpp"
#include <sstream>
#include <iostream>

namespace lance {
std::string http_response::to_string() const
{
    std::ostringstream out;
    out << "HTTP/";
    out << int(_http_major) << "." << int(_http_minor) << " " << int(_status_code) << " " << "\r\n";
    for (auto& header : _headers) {
        out << header.first << ": " << header.second << "\r\n";
    }
    out << "\r\n";
    out << _body;
    return out.str();
}
void http_response::traverse_headers(std::function<void(const std::string&, const std::string&)> func) const
{
    for (auto& header : _headers) {
        func(header.first, header.second);
    }
}
}
