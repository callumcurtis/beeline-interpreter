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


// Initializes the logging system with the given logging level.
// The logging level determines which log messages are printed.
// Log messages with a level less than the given logging level
// are not printed. The output stream is std::clog or std::cerr,
// depending on the severity of the log message.
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


// Returns a LoggingStream with the given logging level.
// Log messages written to the returned LoggingStream are
// printed if their logging level is greater than or equal
// to the given logging level.
LoggingStream log(const LoggingLevel logging_level);
