#ifndef __CONNECTION_HPP
#define __CONNECTION_HPP

#include "http_response.hpp"
#include "response_parser.hpp"

#include <time.h>
#include <asio.hpp>
#include <asio/steady_timer.hpp>
#include <memory>
#include <mutex>
#include <vector>

namespace lance {
class connection : public std::enable_shared_from_this<connection> {
public:
    explicit connection(asio::io_service&, int duration);
    void start();
    void stop();
    int get_requests_num() const { return _requests_num; }
    int get_error_code_num() const { return _error_code_num; }
    uint64_t get_bytes_received() const { return _bytes_received; }
    const std::vector<uint64_t> get_latency() const { return _latency; }
private:
    void start_request();
    void end_request();
    void re_connect();
    void do_connect();
    void do_write();
    void do_read();

    asio::io_service& _io;
    int _duration;
    asio::ip::tcp::socket _sock;
    http_response _response;
    response_parser _parser;
    enum { max_buf = 2048 };
    char _read_buf[max_buf];
    char _write_buf[max_buf];

    timespec _ts_start;
    timespec _ts_end;
    int _requests_num;   //total request num
    int _error_code_num; //non-2xx or 3xx num
    uint64_t _bytes_received;
    std::vector<uint64_t> _latency;

    std::mutex _mutex;
    bool _stopped;
    asio::steady_timer _timer;
};
}

#endif
