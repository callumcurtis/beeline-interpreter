#pragma once

#include <memory>

#include "ast.hpp"


class ExpressionToString : public Expression::Visitor, public Statement::Visitor
{
public:
    ExpressionToString();
    ~ExpressionToString();
    std::string str() const;
    void visit(const Expression::Binary& binary) override;
    void visit(const Expression::Grouping& grouping) override;
    void visit(const Expression::Literal& literal) override;
    void visit(const Expression::Unary& unary) override;
    void visit(const Expression::Variable& variable) override;
    void visit(const Expression::Assignment& assignment) override;
    void visit(const Statement::Expression& expression) override;
    void visit(const Statement::Print& print) override;
    void visit(const Statement::VariableDeclaration& variable_declaration) override;
    void visit(const Statement::Block& block) override;
    void visit(const Statement::IfElse& if_else) override;
    void visit(const Statement::WhileLoop& while_loop) override;
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
