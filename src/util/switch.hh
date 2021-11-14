#ifndef UTIL_SWITCH_HH_
#define UTIL_SWITCH_HH_

#include <functional>
#include <string>

namespace util::switcher {

struct _switch_initializer {
    const char* s;
    std::function<void()> f;
};

void string(const std::string& string, std::initializer_list<_switch_initializer> list, std::function<void()> def = nullptr);

}

#endif