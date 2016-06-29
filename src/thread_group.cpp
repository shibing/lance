#include "thread_group.hpp"

namespace lance {
thread_group::thread_group()
    : _thread_num{std::thread::hardware_concurrency()}
{
}

thread_group::thread_group(size_t thread_num)
    : _thread_num{thread_num}
{
}

void thread_group::run(std::function<void()> func)
{
    for (size_t i = 0; i < _thread_num; ++i) {
        _threads.emplace_back([func]() { func(); });
    }
}

void thread_group::join()
{
    for (auto& t : _threads) {
        t.join();
    }
}
}
