#ifndef LOG_OSTREAMLOGSINK_HH_
#define LOG_OSTREAMLOGSINK_HH_

#include "log.hh"

#include <ostream>
#include <mutex>

namespace logging {

class OstreamLogSink : public LogSinkInterface {
public:
    OstreamLogSink() = delete;
    OstreamLogSink(std::ostream& os);

    void log(const stream& s) override;

private:
    std::ostream& _os;
    std::mutex _mutex;
};

} // namespace logging

#endif
