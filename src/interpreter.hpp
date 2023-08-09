#pragma once

#include <memory>

#include "ast/ast.hpp"


class Interpreter
{
public:
    Interpreter() = delete;
    Interpreter(const std::unique_ptr<Expression> expression);
    ~Interpreter();
    void run();
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
