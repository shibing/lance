#include "lance.hpp"
#include "util.hpp"
#include <iostream>

namespace lance {
lance::lance(size_t conn_num, int duration)
    : _conn_num{conn_num},
      _conns{_conn_num},
      _duration{duration},
      _total_request_num{0},
      _total_error_code_num{0},
      _total_bytes_received{0}
{
}

void lance::run()
{
    for (size_t i = 0; i < _conn_num; ++i) {
        auto conn = std::make_shared<connection>(_io, _duration);
        _conns[i] = conn;
        conn->start();
    }
    _tg.run([this]() {
        _io.run();
    });
    _tg.join();

    for (auto &conn : _conns) {
        _total_request_num += conn->get_requests_num();
        _total_error_code_num += conn->get_error_code_num();
        _total_bytes_received += conn->get_bytes_received();
        for (auto &time : conn->get_latency()) {
            _latency.push_back(time);
        }
    }
    std::sort(_latency.begin(), _latency.end());
}

void lance::output_result() const
{
    if (_total_request_num > 0) {
        std::cout << _total_request_num << " requests, " << util::format_byte(_total_bytes_received, 2) << " read" << std::endl;
        std::cout << "Requests/sec: " << get_request_per_sec() << std::endl;
        std::cout << "Transfer/sec: " << util::format_byte(get_transfer_per_sec(), 2) << std::endl;
        std::cout << "Max latency: " << util::format_time(_latency.back(), 2) << std::endl;
        std::cout << "Min latency: " << util::format_time(_latency.front(), 2) << std::endl;
    }
}
}
