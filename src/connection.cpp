
#include "connection.hpp"
#include <chrono>
#include <functional>
#include <iostream>

namespace lance {

connection::connection(asio::io_service& io, int duration)
    : _io{io},
      _duration{duration},
      _sock{_io},
      _parser{_response},
      _requests_num{0},
      _error_code_num{0},
      _bytes_received{0},
      _stopped{false},
      _timer{_io}
{
}

void connection::start_request()
{
   clock_gettime(CLOCK_MONOTONIC_RAW, &_ts_start);
}

void connection::end_request()
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &_ts_end);
    auto _t = (_ts_end.tv_sec - _ts_start.tv_sec) * 1000000000 + _ts_end.tv_nsec - _ts_start.tv_nsec;
    _latency.push_back(_t);
    //if (_t > 1000000000) {
    //    std::cout << _ts_start.tv_sec << ", " << _ts_start.tv_nsec << std::endl;
    //    std::cout << _ts_end.tv_sec << ", " << _ts_end.tv_nsec << std::endl;
    //}
}

void connection::start()
{
    auto self = shared_from_this();
    _timer.expires_from_now(std::chrono::seconds(_duration));
    _timer.async_wait([this, self](const asio::error_code& error) { this->stop(); });
    do_connect();
}

void connection::stop()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_stopped) return;
    asio::error_code ignored_ec;
    _sock.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
    _sock.close();
    _stopped = true;
}

void connection::re_connect()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_stopped) return;
        _sock.shutdown(asio::ip::tcp::socket::shutdown_both);
        _sock = asio::ip::tcp::socket{_io};
    }
    _parser.init();
    do_connect();
}

void connection::do_connect()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_stopped) return;
    auto self = shared_from_this();
    asio::ip::tcp::endpoint ep{asio::ip::address::from_string("127.0.0.1"), 80};
    start_request();
    _sock.async_connect(ep, [this, self](const asio::error_code& ec) {
        if (!ec) {
            do_write();
        }
    });
}

void connection::do_write()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_stopped) return;
    auto self = shared_from_this();
    const char* msg =
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: Keep-Alive\r\n"
        "\r\n";
    ::strcpy(_write_buf, msg);
    async_write(_sock, asio::buffer(_write_buf, ::strlen(_write_buf)), [this, self](const asio::error_code& ec, size_t bytes_transferred) {
        if (!ec) {
            do_read();
        }
        else {
            std::cerr << "write: " << ec.message() << std::endl;
        }
    });
}

void connection::do_read()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_stopped) return;
    auto self = shared_from_this();
    _sock.async_read_some(asio::buffer(_read_buf), [this, self](const asio::error_code& ec, size_t bytes_transferred) {
        if (ec) {
            if (ec == asio::error::operation_aborted) {
                return;
            }
            if (ec.value() != asio::error::eof) {
                std::cerr << "read: " << ec.message() << std::endl;
                stop();
                return;
            }
            _parser.parse(0, 0);
            if (_parser.get_error() == HPE_PAUSED) {
                _requests_num++;
                if (_response.get_status_code() >= 400) {
                    _error_code_num++;
                }
                _response = http_response();
                re_connect();
                return;
            }
            stop();
            return;
        }

        _bytes_received += bytes_transferred;
        _read_buf[bytes_transferred] = 0;
        size_t bytes_parsed = 0;
        size_t bytes_left = bytes_transferred;
        for (;;) {
            size_t now_bytes_parsed = _parser.parse(_read_buf + bytes_parsed, bytes_left);
            auto error = _parser.get_error();
            if (error != HPE_OK && error != HPE_PAUSED) {
                std::cerr << _parser.get_error_name() << std::endl;
                stop();
                return;
            }
            if (error == HPE_OK) { //not enough data, read again
                break;
            }
            //paused, must one message complete
            ++_requests_num;
            if (_response.get_status_code() >= 400) {
                ++_error_code_num;
            }
            end_request();
            //std::cout << _response.to_string() << std::endl;
            _response = http_response();
            _parser.set_paused(false);
            if (now_bytes_parsed < bytes_left) { //still left message
                //bytes_parsed += now_bytes_parsed;
                //bytes_left -= now_bytes_parsed;
                //continue;
                std::cerr << "no pipeline support yet!" << std::endl;
                stop();
                return;
            }
            if (_parser.is_keep_alive()) {
                start_request();
                do_write();
            }
            else {
                re_connect();
            }
            return;
        }
        do_read();
    });
}
}
