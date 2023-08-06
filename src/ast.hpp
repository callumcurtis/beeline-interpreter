/*
 * This file was generated by the generate_ast.py script.
 */

#pragma once

#include <memory>

#include "lexer.hpp"


struct Expression
{
    class Visitor;
    class Binary;
    class Grouping;
    class Literal;
    class Unary;
    virtual ~Expression() = default;
    virtual void accept(Visitor& visitor) const = 0;
};


struct Expression::Binary : Expression
{
    Binary(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(Expression::Visitor& visitor) const override;
    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;
};


struct Expression::Grouping : Expression
{
    Grouping(std::unique_ptr<Expression> expression);
    void accept(Expression::Visitor& visitor) const override;
    std::unique_ptr<Expression> expression;
};


struct Expression::Literal : Expression
{
    Literal(Token::Literal value);
    void accept(Expression::Visitor& visitor) const override;
    Token::Literal value;
};


struct Expression::Unary : Expression
{
    Unary(Token op, std::unique_ptr<Expression> right);
    void accept(Expression::Visitor& visitor) const override;
    Token op;
    std::unique_ptr<Expression> right;
};


class Expression::Visitor
{
public:
    virtual void visit(const Expression::Binary& binary) = 0;
    virtual void visit(const Expression::Grouping& grouping) = 0;
    virtual void visit(const Expression::Literal& literal) = 0;
    virtual void visit(const Expression::Unary& unary) = 0;
};
