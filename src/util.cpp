#include "util.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace lance {
namespace util {
struct unit_info {
    int scale;
    const char *unit[];
};

static std::string format_unit(long double metric, int precision, const unit_info &unit_info)
{
    const char *const *unit;
    for (unit = unit_info.unit; metric >= unit_info.scale && *unit; ++unit) {
        metric  /= unit_info.scale;
    }
    if (*unit == nullptr) {
        unit -= 1;
        metric *= unit_info.scale;
    }
    std::ostringstream sstream;
    sstream.setf(std::ios::showpoint);
    sstream.precision(precision);
    sstream.setf(std::ios::fixed);
    sstream << metric << *unit;
    return sstream.str();
}

std::string format_time(uint64_t time, int precision)
{
    if (time >= 1000000000) {
        return format_second(time / 1000000000.0, precision);
    }
    return format_nano_second(time, precision);
}

std::string format_byte(uint64_t byte, int precision)
{
    static unit_info byte_unit = {
        1024,
        {"B", "KB", "MB", "GB", "TB", "PB", nullptr}};
    return format_unit(byte, precision, byte_unit);
}

std::string format_nano_second(uint64_t time, int precision)
{
    static unit_info ns_unit = {
        1000,
        {"ns", "us", "ms", nullptr}};
    return format_unit(time, precision, ns_unit);
}

std::string format_second(double time, int precision)
{
    static unit_info sec_unit = {
        60,
        {"s", "min", "h", nullptr}};
    return format_unit(time, precision, sec_unit);
}

} //namsepace util
} //namespace lance
