#include <sstream>
#include <memory>
#include <string>

#include "ast/ast.hpp"
#include "ast/visitor.hpp"


class ExpressionToString::Impl : public Expression::Visitor
{
public:
    std::string result() const
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
private:
    std::ostringstream buffer_{};
};


ExpressionToString::ExpressionToString() : impl_{std::make_unique<Impl>()} {}
ExpressionToString::~ExpressionToString() = default;
std::string ExpressionToString::result() const { return impl_->result(); }
void ExpressionToString::visit(const Expression::Binary& binary) { impl_->visit(binary); }
void ExpressionToString::visit(const Expression::Grouping& grouping) { impl_->visit(grouping); }
void ExpressionToString::visit(const Expression::Literal& literal) { impl_->visit(literal); }
void ExpressionToString::visit(const Expression::Unary& unary) { impl_->visit(unary); }
