#pragma once

#include <vector>

#include "lexer.hpp"


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
