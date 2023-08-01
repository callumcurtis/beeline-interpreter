#pragma once

#include <boost/log/trivial.hpp>


enum struct LoggingLevel
{
    trace,
    debug,
    info,
    warn,
    error,
    fatal,
};


void init_logging(const LoggingLevel logging_level);


class LoggingStream
{
public:
    LoggingStream(const LoggingLevel logging_level);
    template <typename T>
    friend const LoggingStream& operator<<(const LoggingStream& ls, const T& str)
    {
        switch (ls.logging_level_)
        {
            case LoggingLevel::trace:
                BOOST_LOG_TRIVIAL(trace) << str;
                break;
            case LoggingLevel::debug:
                BOOST_LOG_TRIVIAL(debug) << str;
                break;
            case LoggingLevel::info:
                BOOST_LOG_TRIVIAL(info) << str;
                break;
            case LoggingLevel::warn:
                BOOST_LOG_TRIVIAL(warning) << str;
                break;
            case LoggingLevel::error:
                BOOST_LOG_TRIVIAL(error) << str;
                break;
            case LoggingLevel::fatal:
                BOOST_LOG_TRIVIAL(fatal) << str;
                break;
            default:
                assert(false && "invalid logging level");
        }
        return ls;
    }
private:
    LoggingLevel logging_level_;
};


LoggingStream log(const LoggingLevel logging_level);
