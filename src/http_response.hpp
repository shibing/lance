#ifndef __HTTP_RESPONSE_HPP
#define __HTTP_RESPONSE_HPP

#include <vector>
#include <string>
#include <ostream>
#include <cstdint>
#include <map>
#include <functional>
#include <http_parser.h>

namespace lance {

class http_response {
public:
    void set_http_major(uint8_t major) { _http_major = major; }
    void set_http_minor(uint8_t minor) { _http_minor = minor; }
    uint8_t get_http_major() const { return _http_major; }
    uint8_t get_http_minor() const { return _http_minor; }
    void set_status_code(uint16_t status_code) { _status_code = status_code; }
    uint16_t get_status_code() const { return _status_code; }
    template <typename T, typename U>
    bool add_header(T&& name, U&& value)
    {
        if (_headers.find(name) != _headers.end()) {
            return false;
        }
        auto ret = _headers.emplace(std::forward<T>(name), std::forward<U>(value));
        return ret.second;
    }
    std::string& get_header(const std::string& name)
    {
        return _headers.at(name);
    }
    const std::string& get_header(const std::string& name) const
    {
        return _headers.at(name);
    }

    std::string& get_body()
    {
        return _body;
    }

    const std::string& get_body() const
    {
        return _body;
    }

    void set_body(const std::string& body) 
    {
        _body = body;
    }

    void set_body(std::string&& body) 
    {
        _body = std::move(body);
    }

    std::string to_string() const;

    void traverse_headers(std::function<void(const std::string&, const std::string&)>) const;

private:
    uint8_t _http_major;
    uint8_t _http_minor;
    uint16_t _status_code;
    std::map<std::string, std::string> _headers;
    std::string _body;
};
}

#endif
