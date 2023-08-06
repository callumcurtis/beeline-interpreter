#pragma once

#include <memory>

#include "ast/ast.hpp"


class ExpressionToString : public Expression::Visitor
{
public:
    ExpressionToString();
    ~ExpressionToString();
    std::string str() const;
    void visit(const Expression::Binary& binary) override;
    void visit(const Expression::Grouping& grouping) override;
    void visit(const Expression::Literal& literal) override;
    void visit(const Expression::Unary& unary) override;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
