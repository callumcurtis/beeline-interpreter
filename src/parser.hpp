#pragma once

#include <vector>
#include <string>
#include <ostream>

#include "beeline.hpp"


class Parser
{
public:
    Parser() = delete;
    Parser(std::vector<Token> tokens);
    ~Parser();
private:
    class Impl;
    Impl *impl_;
};


class BeelineParseError : public BeelineError
{
public:
    BeelineParseError(const std::string& message, const Token& token);
    const Token token;
};


std::ostream& operator<<(std::ostream& os, const BeelineParseError& bpe);
