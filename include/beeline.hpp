#pragma once

#include <string>
#include <ostream>
#include <stdexcept>


class Beeline
{
public:
    void run(const std::string& input);
};


class BeelineError : public std::runtime_error
{
public:
    BeelineError(const std::string& message);
};


std::ostream& operator<<(std::ostream& os, const BeelineError& be);
