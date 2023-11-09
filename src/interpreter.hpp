#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "beeline.hpp"
#include "lexer.hpp"
#include "ast.hpp"


// Interprets a list of statements.
class Interpreter
{
public:
    Interpreter();
    ~Interpreter();
    // Interprets the given list of statements.
    void interpret(const std::vector<std::unique_ptr<Statement>> statements);
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};


// Exception thrown when an error occurs in the beeline interpreter.
class BeelineRuntimeError : public BeelineError
{
public:
    BeelineRuntimeError(const std::string& message, const Token::Position& position);
    Token::Position position;
};


std::ostream& operator<<(std::ostream& os, const BeelineRuntimeError& bre);
