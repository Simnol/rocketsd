#include "log/ostream_logsink.hh"

#include <map>
#include <iomanip>
#include <optional>

#include <ctime>

#include <util/time.hh>

namespace logging {

namespace {
    const char* LOG_TIME_FORMAT = "%Y-%m-%dT%H:%M:%S";

    const std::string string_from_level(logging::level level)
    {
        static const std::map<const logging::level, const std::string> LOG_LEVEL_STRING = {
            { DEBUG, "debug" },
            { INFO, "info" },
            { WARNING, "warn" },
            { ERR, "error" },
        };

        return LOG_LEVEL_STRING.at(level);
    }

    std::optional<tag_value> find_tag(const stream& s, const std::string& key)
    {
        auto it = s.tags.find(key);
        if (it != s.tags.end()) {
            return std::optional(it->second);
        }

        return std::optional<tag_value>();
    }

    template<typename T>
    std::optional<T> find_tag_typed(const stream& s, const std::string& key)
    {
        if (auto tag = find_tag(s, key)) {
            if (std::holds_alternative<T>(tag.value())) {
                return std::optional<T>(std::get<T>(tag.value()));
            }
        }

        return std::optional<T>();
    }


    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}

OstreamLogSink::OstreamLogSink(std::ostream& os)
    : _os(os)
{
}

void OstreamLogSink::log(const stream& s)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (auto time = find_tag_typed<timepoint>(s, "time")) {
        _os << "[" << util::time::to_string(time.value(), LOG_TIME_FORMAT) << "] ";
    }
    
    _os << "(" << string_from_level(s.level) << ") ";

    if (auto component = find_tag_typed<std::string>(s, "component")) {
        _os << "[" << component.value() << "] ";
    }

    _os << s.message.str() << " ";

    for (const auto& tag: s.tags) {
        if (tag.first == "time" || tag.first == "component") {
            continue;
        }
        _os << tag.first << "=";
        std::visit(overloaded {
            [this](auto arg) { _os << arg; },
            [this](std::chrono::system_clock::time_point arg) {
                _os << util::time::to_string(arg, LOG_TIME_FORMAT);
            }
        }, tag.second);
        _os << " ";
    }

    _os << std::endl;
}

} // namespace logging