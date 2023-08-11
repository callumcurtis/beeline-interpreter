#pragma once

#include <memory>
#include <string>

#include "lexer.hpp"


// Environment for storing and retrieving the current state of variables.
// Environments can be nested to support scoping.
class Environment
{
public:
    Environment();
    ~Environment();
    Environment(const Environment& other);
    Environment(Environment&& other);
    Environment& operator=(const Environment& other);
    Environment& operator=(Environment&& other);
    // Nests the current environment within the given parent environment.
    Environment& nested(Environment& parent);
    // Defines a new variable with the given name and value in the current environment.
    void define(const std::string& name, const Token::Literal& value, const Token::Position& position);
    // Assigns the given value to the variable with the given name. Cascades to the
    // parent environment if the variable is not defined in the current environment.
    void assign(const std::string& name, const Token::Literal& value, const Token::Position& position);
    // Returns the value of the variable with the given name. Cascades to the parent
    // environment if the variable is not defined in the current environment.
    Token::Literal get(const std::string& name, const Token::Position& position) const;
private:
    // PIMPL idiom
    class Impl;
    std::unique_ptr<Impl> impl_;
};
