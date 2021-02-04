#ifndef LOG_H_
#define LOG_H_

#include <iostream>
#include <string>
#include <mutex>

class Log {
public:
    enum Level {
        DEBUG = -1,
        INFO = 0,
        WARNING = 1,
        ERROR = 2
    };

private:
    std::ostream* stream_;
    std::mutex mutex_;

public:
    static Log& get();

    static std::ostream& debug(std::string component, std::string message = "");
    static std::ostream& info(std::string component, std::string message = "");
    static std::ostream& warn(std::string component, std::string message = "");
    static std::ostream& err(std::string component, std::string message = "");

public:
    ~Log();

    std::ostream& log(Level level, std::string component, std::string message);

    void setStream(std::ostream& stream) { stream_ = &stream; }

private:
    Log();
};

#endif