#ifndef UTIL_TIME_HH_
#define UTIL_TIME_HH_

#include <chrono>
#include <ratio>

#include <cstdint>

namespace util {

/** now<Unit>
 * Returns the current time in `Unit` seconds since epoch. `Unit` should be one
 * of the std::ratio classes.
 * 
 * @return time in `Unit`-seconds since epoch
 */
template <class Unit>
std::uint64_t now()
{
    return std::chrono::duration_cast<std::chrono::duration<std::uint64_t, Unit>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

}

#endif