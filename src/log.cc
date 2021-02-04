#include "log.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <thread>

#include <ctime>

namespace {
const char* LOG_TIME_FORMAT = "%Y-%m-%d %H:%M:%S";

const std::string string_from_level(Log::Level level)
{
    static const std::map<const Log::Level, const std::string> LOG_LEVEL_STRING = {
        { Log::DEBUG, "debug" },
        { Log::INFO, "info" },
        { Log::WARNING, "warning" },
        { Log::ERROR, "error" },
    };

    return LOG_LEVEL_STRING.at(level);
}

}

std::ostream& Log::debug(std::string component, std::string message)
{
    return Log::get().log(DEBUG, component, message);
}

std::ostream& Log::info(std::string component, std::string message)
{
    return Log::get().log(INFO, component, message);
}

std::ostream& Log::warn(std::string component, std::string message)
{
    return Log::get().log(WARNING, component, message);
}

std::ostream& Log::err(std::string component, std::string message)
{
    return Log::get().log(ERROR, component, message);
}

Log::Log()
{
    stream_ = nullptr;
}

Log::~Log() {}

Log& Log::get()
{
    static Log log;
    return log;
}

std::ostream& Log::log(Level level, std::string component, std::string message)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::thread::id thread = std::this_thread::get_id();
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::ostream& out = stream_ ? *stream_ : std::cout;

    out << "[" << std::put_time(std::localtime(&now), LOG_TIME_FORMAT) << "] ";
    out << "(" << string_from_level(level) << ") ";
    out << "<thread " << std::hex << thread << std::dec << "> ";
    out << "[" << component << "] ";

    if (message != "") {
        out << message << std::endl;
    }

    return out;
}
