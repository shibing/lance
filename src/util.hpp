#ifndef __UTIL_HPP
#define __UTIL_HPP

#include <cstdint>
#include <string>

namespace lance {
namespace util {

struct unit_info;

static std::string format_unit(long double metric, int precision, const unit_info&);
std::string format_byte(uint64_t byte, int precision);
std::string format_time(uint64_t time, int precision);
std::string format_nano_second(uint64_t time, int precision);
std::string format_second(double time, int precision);
}
}

#endif
