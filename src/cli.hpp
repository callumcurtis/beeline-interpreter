#pragma once


enum struct DebugLevel
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
    const DebugLevel debug_level;
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
