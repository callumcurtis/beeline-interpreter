#include <vector>
#include <memory>
#include <initializer_list>
#include <algorithm>

#include "parser.hpp"
#include "ast.hpp"
#include "lexer.hpp"


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
    std::unique_ptr<Expression> expression()
    {
        return equality();
    }
    std::unique_ptr<Expression> equality()
    {
        std::unique_ptr<Expression> expression = comparison();
        while (is_match({Token::Type::BANG_EQUAL, Token::Type::EQUAL_EQUAL}))
        {
            const Token& op = advance();
            std::unique_ptr<Expression> right = comparison();
            expression = std::make_unique<Expression::Binary>(std::move(expression), op, std::move(right));
        }
        return expression;
    }
};
