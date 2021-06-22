#ifndef LOG_LOG_HH_
#define LOG_LOG_HH_

#include <chrono>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_map>
#include <variant>

namespace logging {

using timepoint = std::chrono::system_clock::time_point;
using tag_value = std::variant<int, double, timepoint, std::string>;

enum level {
    DEBUG = -1,
    INFO = 0,
    WARNING = 1,
    ERR = 2,
    MAX = 99
};

struct tag {
    std::string key;
    tag_value value;
};

struct stream {
    logging::level level;
    std::stringstream message;
    std::unordered_map<std::string, tag_value> tags;
};

class LogSinkInterface {
public:
    virtual void log(const stream& stream) = 0;
};

class Log {
public:
    static Log& get();

public:
    ~Log();

    void log(const stream& stream);
    void addSink(LogSinkInterface* sink);
    void removeSink(LogSinkInterface* sink);

private:
    Log();

private:
    std::vector<LogSinkInterface*> _sinks; // FIXME: use weak_pointer
};

extern std::function<void(stream&)> endl;

stream debug(const std::string& component);
stream info(const std::string& component);
stream warn(const std::string& component);
stream err(const std::string& component);

} // namespace logging

logging::stream&& operator<<(logging::stream&& s, std::function<void(logging::stream&)> manip);
logging::stream&& operator<<(logging::stream&& s, logging::tag&& tag);

template<typename T>
logging::stream&& operator<<(logging::stream&& s, const T& t)
{
    s.message << t;
    return std::move(s);
}

#endif // LOG_HH_