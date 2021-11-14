#include "log/log.hh"

#include <util/time.hh>

namespace logging {

namespace {
    stream create_log_stream(logging::level level, const std::string& component)
    {        
        stream s;
        s.level = level;
        s.tags.emplace("component", component);
        s.tags.emplace("time", std::chrono::system_clock::now());
        return s;
    }
}

std::function<void(stream&)> endl = [](stream& s) {
    Log::get().log(s);
    s.message.str("");
};

stream debug(const std::string& component)
{
    return create_log_stream(DEBUG, component);
}

stream info(const std::string& component)
{
    return create_log_stream(INFO, component);
}

stream warn(const std::string& component)
{
    return create_log_stream(WARNING, component);
}

stream err(const std::string& component)
{
    return create_log_stream(ERR, component);
}

Log::Log()
{
}

Log::~Log() {}

Log& Log::get()
{
    static Log log;
    return log;
}

void Log::log(const stream& s)
{
    for (auto sink: _sinks) {
        sink->log(s);
    }
}

void Log::addSink(LogSinkInterface* sink)
{
    _sinks.push_back(sink);
}

void Log::removeSink(LogSinkInterface* sink)
{
    auto it = std::find(_sinks.begin(), _sinks.end(), sink);
    if (it != _sinks.end()) {
        _sinks.erase(it);
    }
}

} // namespace logging

logging::stream&& operator<<(logging::stream&& s, std::function<void(logging::stream&)> manip)
{
    manip(s);
    return std::move(s);
}

logging::stream&& operator<<(logging::stream&& s, logging::tag&& tag)
{
    s.tags.insert_or_assign(tag.key, tag.value);
    return std::move(s);
}
