#pragma once

#include <string>
#include <ostream>
#include <stdexcept>


// Beeline interpreter.
class Beeline
{
public:
    // Runs the beeline interpreter on the given input.
    void run(const std::string& input);
};


// Exception thrown when an error occurs in the beeline interpreter.
// Internal errors are caught and propagated to the user as BeelineErrors.
class BeelineError : public std::runtime_error
{
public:
    BeelineError(const std::string& message);
};


std::ostream& operator<<(std::ostream& os, const BeelineError& be);
