#ifndef __RESPONSE_PARSER_HPP
#define __RESPONSE_PARSER_HPP

#include <http_parser.h>

#include "http_response.hpp"

namespace lance {
class response_parser {
public:
    response_parser(http_response &response);
    void init();
    size_t parse(const char *data, size_t len);
    void set_paused(bool is_paused);
    bool is_keep_alive() const;
    bool is_message_complete() const { return _message_complete; }
    int get_error() const { return _parser.http_errno; }
    std::string get_error_name() const { return http_errno_name(http_errno(_parser.http_errno)); }
private:
    static int status_handler(http_parser *parser, const char *at, size_t len);
    static int header_field_handler(http_parser *parser, const char *at, size_t len);
    static int header_value_handler(http_parser *parser, const char *at, size_t len);
    static int header_complete_handler(http_parser *parser);
    static int body_handler(http_parser *parser, const char *at, size_t len);
    static int message_complete_handler(http_parser *parser);

    bool _message_complete;
    http_parser_settings _settings;
    http_parser _parser;
    http_response &_response;
    std::string _name_buf;
    std::string _value_buf;
    std::string _body_buf;
};
}

#endif
