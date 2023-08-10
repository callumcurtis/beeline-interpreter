#pragma once

#include <memory>
#include <ostream>
#include <string>

#include "beeline.hpp"
#include "lexer.hpp"
#include "ast/ast.hpp"


class Interpreter
{
public:
    Interpreter();
    ~Interpreter();
    void interpret(std::unique_ptr<Expression> expression);
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};


class BeelineRuntimeError : public BeelineError
{
public:
    BeelineRuntimeError(const std::string& message, const Token& token);
    Token token;
};


std::ostream& operator<<(std::ostream& os, const BeelineRuntimeError& bre);
