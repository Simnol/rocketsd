#ifndef UTIL_TIME_HH_
#define UTIL_TIME_HH_

#include <chrono>
#include <ratio>

#include <cstdint>

namespace util::time {

/** now<Unit>
 * Returns the current time in `Unit` seconds since epoch. `Unit` should be one
 * of the std::ratio classes.
 * 
 * @return time in `Unit`-seconds since epoch
 */
template <class Unit>
std::uint64_t now()
{
    return std::chrono::duration_cast<std::chrono::duration<std::uint64_t, Unit>>(std::chrono::system_clock::now().time_since_epoch()).count();
}

template <class Clock>
std::string to_string(std::chrono::time_point<Clock> tp, const std::string& format)
{
    char buf[64] = { 0 };
    struct tm tm_buffer;

#if (defined _MSC_VER)
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::clock_cast<std::chrono::system_clock>(tp));
    localtime_s(&tm_buffer, &now);
#elif __GNUC__
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() + (tp - Clock::now()));
    localtime_r(&now, &tm_buffer);
#endif

    strftime(buf, sizeof(buf), format.c_str(), &tm_buffer);
    return std::string(buf);
}

} // namespaces

#endif