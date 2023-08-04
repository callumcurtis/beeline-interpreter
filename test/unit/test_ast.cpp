#include <catch2/catch.hpp>

#include <sstream>

#include "ast.hpp"


class ExpressionToStringVisitor : public Expression::Visitor
{
public:
    std::string result() const
    {
        return result_.str();
    }
    void visit(const Expression::Binary& binary) override
    {
        result_ << "(";
        binary.left->accept(*this);
        result_ << " " << binary.op.lexeme << " ";
        binary.right->accept(*this);
        result_ << ")";
    }
    void visit(const Expression::Grouping& grouping) override
    {
        result_ << "(";
        grouping.expression->accept(*this);
        result_ << ")";
    }
    void visit(const Expression::Literal& literal) override
    {
        result_ << literal.value;
    }
    void visit(const Expression::Unary& unary) override
    {
        result_ << "(" << unary.op.lexeme << " ";
        unary.right->accept(*this);
        result_ << ")";
    }
private:
    std::ostringstream result_{};
};


TEST_CASE("visitor")
{
    SECTION("expression")
    {
        std::unique_ptr<Expression> expression = std::make_unique<Expression::Binary>(
            std::make_unique<Expression::Unary>(
                Token{Token::Type::MINUS, "-", nullptr, Token::Position{0, 0, 0, 0}},
                std::make_unique<Expression::Literal>(149.84)
            ),
            Token{Token::Type::STAR, "*", nullptr, Token::Position{0, 0, 0, 0}},
            std::make_unique<Expression::Grouping>(
                std::make_unique<Expression::Literal>(true)
            )
        );
        ExpressionToStringVisitor visitor;
        expression->accept(visitor);
        REQUIRE(visitor.result() == "((- 149.840000) * (true))");
    }
}