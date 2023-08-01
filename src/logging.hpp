#pragma once

#include <boost/log/trivial.hpp>


enum struct LoggingLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
};


void init_logging(const LoggingLevel logging_level);


class LoggingStream
{
public:
    LoggingStream(const LoggingLevel logging_level);
    template <typename String>
    friend const LoggingStream& operator<<(const LoggingStream& ls, const String& str)
    {
        switch (ls.logging_level_)
        {
            case LoggingLevel::TRACE:
                BOOST_LOG_TRIVIAL(trace) << str;
                break;
            case LoggingLevel::DEBUG:
                BOOST_LOG_TRIVIAL(debug) << str;
                break;
            case LoggingLevel::INFO:
                BOOST_LOG_TRIVIAL(info) << str;
                break;
            case LoggingLevel::WARN:
                BOOST_LOG_TRIVIAL(warning) << str;
                break;
            case LoggingLevel::ERROR:
                BOOST_LOG_TRIVIAL(error) << str;
                break;
            case LoggingLevel::FATAL:
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
