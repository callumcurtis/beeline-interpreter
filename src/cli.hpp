#pragma once


enum struct LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
};


struct Arguments
{
    const LogLevel log_level;
    const bool version;
    const bool help;
};


class ArgumentParser
{
public:
    ArgumentParser();
    ~ArgumentParser();
    Arguments parse(const int argc, const char** argv);
private:
    class Impl;
    Impl *impl_;
};
