#pragma once

#include "logging.hpp"


struct Arguments
{
    const LoggingLevel logging_level;
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
