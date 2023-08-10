#include <sstream>
#include <memory>
#include <string>

#include "ast/ast.hpp"
#include "ast/stringify.hpp"


class ExpressionToString::Impl : public Expression::Visitor, public Statement::Visitor
{
public:
    std::string str() const
    {
        return buffer_.str();
    }
    void visit(const Expression::Binary& binary) override
    {
        buffer_ << "(";
        binary.left->accept(*this);
        buffer_ << " " << binary.op.lexeme << " ";
        binary.right->accept(*this);
        buffer_ << ")";
    }
    void visit(const Expression::Grouping& grouping) override
    {
        buffer_ << "(";
        grouping.expression->accept(*this);
        buffer_ << ")";
    }
    void visit(const Expression::Literal& literal) override
    {
        buffer_ << literal.value;
    }
    void visit(const Expression::Unary& unary) override
    {
        buffer_ << "(" << unary.op.lexeme << " ";
        unary.right->accept(*this);
        buffer_ << ")";
    }
    void visit(const Expression::Variable& variable) override
    {
        buffer_ << variable.name.lexeme;
    }
    void visit(const Statement::Expression& expression) override
    {
        expression.expression->accept(*this);
    }
    void visit(const Statement::Print& print) override
    {
        buffer_ << "(print ";
        print.expression->accept(*this);
        buffer_ << ")";
    }
    void visit(const Statement::VariableDeclaration& variable_declaration) override
    {
        buffer_ << "(var " << variable_declaration.name.lexeme << " = ";
        variable_declaration.initializer->accept(*this);
        buffer_ << ")";
    }
private:
    std::ostringstream buffer_{};
};


ExpressionToString::ExpressionToString() : impl_{std::make_unique<Impl>()} {}
ExpressionToString::~ExpressionToString() = default;
std::string ExpressionToString::str() const { return impl_->str(); }
void ExpressionToString::visit(const Expression::Binary& binary) { impl_->visit(binary); }
void ExpressionToString::visit(const Expression::Grouping& grouping) { impl_->visit(grouping); }
void ExpressionToString::visit(const Expression::Literal& literal) { impl_->visit(literal); }
void ExpressionToString::visit(const Expression::Unary& unary) { impl_->visit(unary); }
void ExpressionToString::visit(const Expression::Variable& variable) { impl_->visit(variable); }
void ExpressionToString::visit(const Statement::Expression& expression) { impl_->visit(expression); }
void ExpressionToString::visit(const Statement::Print& print) { impl_->visit(print); }
void ExpressionToString::visit(const Statement::VariableDeclaration& variable_declaration) { impl_->visit(variable_declaration); }
