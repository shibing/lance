#ifndef __THREAD_GROUP_HPP
#define __THREAD_GROUP_HPP

#include <functional>
#include <thread>
#include <vector>

namespace lance
{
    class thread_group {
    public:
        thread_group();
        explicit thread_group(size_t thread_num);
        void run(std::function<void ()>);
        void join();
        size_t get_thread_num() const { return _thread_num; }
    private:
        size_t _thread_num;
        std::vector<std::thread> _threads;
    };
}

#endif
