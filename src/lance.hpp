#ifndef __LANCE_HPP
#define __LANCE_HPP

#include "connection.hpp"
#include "thread_group.hpp"
#include <asio.hpp>
#include <memory>
#include <vector>

namespace lance {
class lance {
public:
    typedef std::shared_ptr<connection> connection_ptr;
    lance(size_t conn_num, int duration);
    void run();

    uint64_t get_total_request_num() const { return _total_request_num; }
    uint64_t get_total_error_code_num() const { return _total_error_code_num; }
    uint64_t get_total_bytes_received() const { return _total_bytes_received; }
    double get_request_per_sec() const { return _total_request_num / (1.0 * _duration); }
    double get_transfer_per_sec() const { return _total_bytes_received / (1.0 * _duration); }

    void output_result() const ;

private:
    size_t _conn_num;
    std::vector<connection_ptr> _conns;
    int _duration;
    thread_group _tg;
    asio::io_service _io;

    uint64_t _total_request_num;
    uint64_t _total_error_code_num;
    uint64_t _total_bytes_received;
    std::vector<uint64_t> _latency;
};
}

#endif
