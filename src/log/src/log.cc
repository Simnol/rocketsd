#include "log/log.hh"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <thread>

#include <util/time.hh>

#include <ctime>

namespace {
const char* LOG_TIME_FORMAT = "%Y-%m-%d %H:%M:%S";

const std::string string_from_level(Log::Level level)
{
    static const std::map<const Log::Level, const std::string> LOG_LEVEL_STRING = {
        { Log::DEBUG, "debug" },
        { Log::INFO, "info" },
        { Log::WARNING, "warn" },
        { Log::ERROR, "error" },
    };

    return LOG_LEVEL_STRING.at(level);
}

}

std::ostream& Log::debug(const std::string& component, const std::string& message)
{
    return Log::get().log(DEBUG, component, message);
}

std::ostream& Log::info(const std::string& component, const std::string& message)
{
    return Log::get().log(INFO, component, message);
}

std::ostream& Log::warn(const std::string& component, const std::string& message)
{
    return Log::get().log(WARNING, component, message);
}

std::ostream& Log::err(const std::string& component, const std::string& message)
{
    return Log::get().log(ERROR, component, message);
}

void Log::setStream(std::ostream* stream)
{
    Log::get().stream = stream;
}

Log::Log()
{
    stream = nullptr;
}

Log::~Log() {}

Log& Log::get()
{
    static Log log;
    return log;
}

std::ostream& Log::log(Level level, const std::string& component, const std::string& message)
{
    std::lock_guard<std::mutex> lock(mutex);
    std::ostream* os = stream ? stream : &std::cout;
    std::thread::id thread = std::this_thread::get_id();
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    struct tm tm_buffer;
    localtime_s(&tm_buffer, &now);

    *os << "[" << util::time::to_string(std::chrono::system_clock::now(), LOG_TIME_FORMAT) << "] ";
    *os << "(" << string_from_level(level) << ") ";
    *os << "<thread " << std::hex << thread << std::dec << "> ";
    *os << "[" << component << "] ";

    if (message != "") {
        *os << message << std::endl;
    }

    return *os;
}
