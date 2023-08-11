#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <memory>

#include "beeline.hpp"
#include "lexer.hpp"
#include "ast.hpp"


// Parses a list of tokens into a list of statements.
class Parser
{
public:
    Parser() = delete;
    Parser(const std::vector<Token>& tokens);
    ~Parser();
    // Parses the list of tokens into a list of statements.
    std::vector<std::unique_ptr<Statement>> parse();
private:
    // PIMPL idiom
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
