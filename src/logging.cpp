#include <cassert>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

#include "logging.hpp"


namespace logging = boost::log;


logging::trivial::severity_level to_boost_logging_level(const LoggingLevel logging_level)
{
    switch (logging_level)
    {
        case LoggingLevel::trace: return logging::trivial::trace;
        case LoggingLevel::debug: return logging::trivial::debug;
        case LoggingLevel::info: return logging::trivial::info;
        case LoggingLevel::warn: return logging::trivial::warning;
        case LoggingLevel::error: return logging::trivial::error;
        case LoggingLevel::fatal: return logging::trivial::fatal;
    }
    assert(false && "invalid logging level");
}


void init_logging(const LoggingLevel logging_level)
{
    logging::core::get()->set_filter(
        logging::trivial::severity >= to_boost_logging_level(logging_level)
    );
}


LoggingStream::LoggingStream(const LoggingLevel logging_level) : logging_level_{logging_level} {}


LoggingStream log(const LoggingLevel logging_level)
{
    return LoggingStream{logging_level};
}
