#pragma once

#include <vector>
#include <variant>
#include <memory>
#include <cstddef>

#include "beeline.hpp"


struct Token
{
    enum struct Type
    {
        // Single-character tokens.
        LEFT_PARENTHESIS,
        RIGHT_PARENTHESIS,
        LEFT_BRACE,
        RIGHT_BRACE,
        MINUS,
        PLUS,
        SLASH,
        STAR,
        NEWLINE,

        // One or two character tokens.
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // Literals.
        IDENTIFIER,
        STRING,
        NUMBER,

        // Keywords.
        AND,
        OR,
        IF,
        ELSE,
        TRUE,
        FALSE,
        NIL,
        PRINT,
        VAR,
        WHILE,

        // End of file.
        END_OF_FILE,
    };

    struct Position
    {
        std::size_t offset;
        std::size_t line;
        std::size_t column;
        std::size_t length;
    };

    using Literal = std::variant<std::nullptr_t, std::string, double, bool>;

    Type type;
    std::string lexeme;
    Literal literal;
    Position position;
};


class Lexer
{
public:
    Lexer() = delete;
    Lexer(const std::string& input);
    ~Lexer();
    const std::vector<Token>& scan();
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};


class BeelineSyntaxError : public BeelineError
{
public:
    BeelineSyntaxError(const std::string& message, const Token::Position& position);
    Token::Position position;
};


class BeelineLexingError : public BeelineError
{
public:
    BeelineLexingError(const std::string& message);
};


std::ostream& operator<<(std::ostream& os, const Token::Type& type);
std::ostream& operator<<(std::ostream& os, const Token::Position& position);
std::ostream& operator<<(std::ostream& os, const Token::Literal& literal);
std::ostream& operator<<(std::ostream& os, const Token& token);
std::ostream& operator<<(std::ostream& os, const BeelineSyntaxError& bse);
std::ostream& operator<<(std::ostream& os, const BeelineLexingError& ble);
