#include <vector>
#include <memory>
#include <initializer_list>
#include <algorithm>
#include <cassert>

#include "parser.hpp"
#include "ast.hpp"
#include "lexer.hpp"


enum struct Associativity
{
    LEFT,
    RIGHT,
};


class Parser::Impl
{
public:
    Impl(std::vector<Token> tokens) : tokens_(tokens) {}
private:
    const std::vector<Token> tokens_;
    std::size_t current_token_index_{0};
    bool is_match(const std::initializer_list<Token::Type> types) const
    {
        return std::any_of(
            types.begin(),
            types.end(),
            [this](const Token::Type type) { return is_match(type); }
        );
    }
    bool is_match(const Token::Type type) const
    {
        return (!is_done()) && (peek().type == type);
    }
    bool is_done() const
    {
        return peek().type == Token::Type::END_OF_FILE;
    }
    const Token& peek() const
    {
        return peek(0);
    }
    const Token& peek(const std::size_t ahead) const
    {
        return tokens_.at(current_token_index_ + ahead);
    }
    const Token& advance()
    {
        return tokens_.at(current_token_index_++);
    }
    std::unique_ptr<Expression> binary(
        std::unique_ptr<Expression> (Parser::Impl::*operand)(void),
        const Associativity associativity,
        const std::initializer_list<Token::Type> types
    ) {
        assert(associativity == Associativity::LEFT && "Associativity::RIGHT is not implemented");
        std::unique_ptr<Expression> expression = (this->*operand)();
        while (is_match(types))
        {
            const Token& op = advance();
            const std::unique_ptr<Expression> right = (this->*operand)();
            expression = std::make_unique<Expression::Binary>(std::move(expression), op, std::move(right));
        }
        return expression;
    }
    std::unique_ptr<Expression> expression()
    {
        return equality();
    }
    std::unique_ptr<Expression> equality()
    {
        return binary(&comparison, Associativity::LEFT, {Token::Type::BANG_EQUAL, Token::Type::EQUAL_EQUAL});
    }
    std::unique_ptr<Expression> comparison()
    {
        return binary(&term, Associativity::LEFT, {Token::Type::GREATER, Token::Type::GREATER_EQUAL, Token::Type::LESS, Token::Type::LESS_EQUAL});
    }
    std::unique_ptr<Expression> term()
    {
        return binary(&factor, Associativity::LEFT, {Token::Type::MINUS, Token::Type::PLUS});
    }
    std::unique_ptr<Expression> factor()
    {
        return binary(&unary, Associativity::LEFT, {Token::Type::SLASH, Token::Type::STAR});
    }
    std::unique_ptr<Expression> unary()
    {
        if (is_match({Token::Type::BANG, Token::Type::MINUS}))
        {
            const Token& op = advance();
            const std::unique_ptr<Expression> right = unary();
            return std::make_unique<Expression::Unary>(op, std::move(right));
        }
        return primary();
    }
};
