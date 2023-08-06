#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <memory>

#include "beeline.hpp"
#include "lexer.hpp"
#include "ast/ast.hpp"


class Parser
{
public:
    Parser() = delete;
    Parser(const std::vector<Token>& tokens);
    ~Parser();
    std::unique_ptr<Expression> parse();
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};


class BeelineParseError : public BeelineError
{
public:
    BeelineParseError(const std::string& message, const Token& token);
    Token token;
};


std::ostream& operator<<(std::ostream& os, const BeelineParseError& bpe);
