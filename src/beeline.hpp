#pragma once

#include <string>
#include <ostream>
#include <vector>
#include <stdexcept>
#include <variant>
#include <memory>
#include <cstddef>


class BeelineError : public std::runtime_error
{
public:
    BeelineError(const std::string& message);
};


class BeelineSyntaxError : public BeelineError
{
public:
    BeelineSyntaxError(const std::string& message, const Token::Position& position);
    const Token::Position& position() const noexcept;
private:
    const Token::Position position_;
};


class BeelineLexingError : public BeelineError
{
public:
    BeelineLexingError(const std::string& message);
};


struct Token
{
    enum class Type
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
        const std::size_t offset;
        const std::size_t line;
        const std::size_t column;
        const std::size_t length;
    };

    using Literal = std::variant<std::nullptr_t, std::string, double, bool>;

    const Type type;
    const std::string lexeme;
    const Literal literal;
    const Position position;
};


std::ostream& operator<<(std::ostream& os, const Token::Type& type);
std::ostream& operator<<(std::ostream& os, const Token::Position& position);
std::ostream& operator<<(std::ostream& os, const Token& token);
std::ostream& operator<<(std::ostream& os, const BeelineError& be);
std::ostream& operator<<(std::ostream& os, const BeelineSyntaxError& bse);
std::ostream& operator<<(std::ostream& os, const BeelineLexingError& ble);


class Lexer
{
public:
    Lexer() = delete;
    Lexer(const std::string& input);
    const std::vector<Token>& scan();
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};


class Beeline
{
public:
    void run(const std::string& input);
};
