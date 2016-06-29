#include "response_parser.hpp"
#include <iostream>

namespace lance {
response_parser::response_parser(http_response &response)
    : _message_complete(false),
      _response(response)
{
    _settings = {0};
    _settings.on_header_field = header_field_handler;
    _settings.on_header_value = header_value_handler;
    _settings.on_headers_complete = header_complete_handler;
    _settings.on_body = body_handler;
    _settings.on_message_complete = message_complete_handler;
    init();
}

void response_parser::init()
{
    http_parser_init(&_parser, HTTP_RESPONSE);
    _parser.data = this;
}

size_t response_parser::parse(const char *data, size_t len)
{
    return http_parser_execute(&_parser, &_settings, data, len);
}

bool response_parser::is_keep_alive() const
{
    return http_should_keep_alive(&_parser);
}

void response_parser::set_paused(bool is_paused) 
{
    int paused = is_paused ? 1 : 0;
    http_parser_pause(&_parser, paused);
}

int response_parser::header_field_handler(http_parser *parser, const char *at, size_t len)
{
    response_parser *self = static_cast<response_parser *>(parser->data);
    http_response &rep = self->_response;

    if (!(self->_value_buf.empty())) { //new header
        rep.add_header(std::move(self->_name_buf), std::move(self->_value_buf));
        self->_name_buf = std::string();
        self->_value_buf = std::string();
    }
    self->_name_buf += std::string(at, len);
    return 0;
}

int response_parser::header_value_handler(http_parser *parser, const char *at, size_t len)
{
    response_parser *self = static_cast<response_parser *>(parser->data);
    self->_value_buf += std::string(at, len);
    return 0;
}

int response_parser::header_complete_handler(http_parser *parser)
{
    response_parser *self = static_cast<response_parser *>(parser->data);
    if (!self->_value_buf.empty()) {
        self->_response.add_header(std::move(self->_name_buf), std::move(self->_value_buf));
    }
    self->_response.set_http_major(parser->http_major);
    self->_response.set_http_minor(parser->http_minor);
    self->_response.set_status_code(parser->status_code);

    return 0;
}

int response_parser::body_handler(http_parser *parser, const char *at, size_t len)
{
    response_parser *self = static_cast<response_parser *>(parser->data);
    self->_body_buf += std::string(at, len);
    return 0;
}
int response_parser::message_complete_handler(http_parser *parser)
{
    response_parser *self = static_cast<response_parser *>(parser->data);
    self->_response.set_body(std::move(self->_body_buf));
    self->_message_complete = true;
    self->_body_buf = std::string();
    self->set_paused(true);
    return 0;
}
}
