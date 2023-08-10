#pragma once

#include <memory>
#include <string>

#include "lexer.hpp"


class Environment
{
public:
    Environment();
    ~Environment();
    void define(const std::string& name, const Token::Literal& value, const Token::Position& position);
    void assign(const Token& name, const Token::Literal& value);
    Token::Literal& get(const std::string& name, const Token::Position& position);
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
