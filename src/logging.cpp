#include <cassert>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "logging.hpp"


namespace logging = boost::log;


logging::trivial::severity_level to_boost_logging_level(const LoggingLevel logging_level)
{
    switch (logging_level)
    {
        case LoggingLevel::TRACE: return logging::trivial::trace;
        case LoggingLevel::DEBUG: return logging::trivial::debug;
        case LoggingLevel::INFO: return logging::trivial::info;
        case LoggingLevel::WARN: return logging::trivial::warning;
        case LoggingLevel::ERROR: return logging::trivial::error;
        case LoggingLevel::FATAL: return logging::trivial::fatal;
    }
    assert(false && "invalid logging level");
}


void init_logging(const LoggingLevel logging_level)
{
    // sets the output stream to std::clog
    logging::add_console_log();
    logging::core::get()->set_filter(
        logging::trivial::severity >= to_boost_logging_level(logging_level)
    );
}


LoggingStream::LoggingStream(const LoggingLevel logging_level) : logging_level_{logging_level} {}


LoggingStream log(const LoggingLevel logging_level)
{
    return LoggingStream{logging_level};
}
