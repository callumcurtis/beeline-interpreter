#include <sstream>
#include <memory>
#include <string>

#include "ast.hpp"
#include "stringify.hpp"


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
    void visit(const Expression::Assignment& assignment) override
    {
        buffer_ << "(" << assignment.name.lexeme << " = ";
        assignment.value->accept(*this);
        buffer_ << ")";
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
        buffer_ << "(var " << variable_declaration.name.lexeme;
        if (variable_declaration.initializer)
        {
            buffer_ << " = ";
            variable_declaration.initializer->accept(*this);
        }
        buffer_ << ")";
    }
    void visit(const Statement::Block& block) override
    {
        buffer_ << "{";
        for (const auto& statement : block.statements)
        {
            statement->accept(*this);
            buffer_ << " ";
        }
        buffer_ << "}";
    }
    void visit(const Statement::IfElse& if_else) override
    {
        buffer_ << "(if ";
        if_else.condition->accept(*this);
        buffer_ << " then ";
        if_else.then_statement->accept(*this);
        if (if_else.else_statement)
        {
            buffer_ << " else ";
            if_else.else_statement->accept(*this);
        }
        buffer_ << ")";
    }
    void visit(const Statement::WhileLoop& while_loop) override
    {
        buffer_ << "(while ";
        while_loop.condition->accept(*this);
        buffer_ << " do ";
        while_loop.body->accept(*this);
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
void ExpressionToString::visit(const Expression::Assignment& assignment) { impl_->visit(assignment); }
void ExpressionToString::visit(const Statement::Expression& expression) { impl_->visit(expression); }
void ExpressionToString::visit(const Statement::Print& print) { impl_->visit(print); }
void ExpressionToString::visit(const Statement::VariableDeclaration& variable_declaration) { impl_->visit(variable_declaration); }
void ExpressionToString::visit(const Statement::Block& block) { impl_->visit(block); }
void ExpressionToString::visit(const Statement::IfElse& if_else) { impl_->visit(if_else); }
void ExpressionToString::visit(const Statement::WhileLoop& while_loop) { impl_->visit(while_loop); }
