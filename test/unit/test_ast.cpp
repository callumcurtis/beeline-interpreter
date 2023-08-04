#include <catch2/catch.hpp>

#include <sstream>

#include "ast.hpp"


class ExpressionToStringVisitor : public Expression::Visitor
{
public:
    std::string result() const
    {
        return result_;
    }
    void visit(const Expression::Binary& binary) override
    {
        std::ostringstream oss;
        oss << "(";
        binary.left->accept(*this);
        oss << " " << binary.op.lexeme << " ";
        binary.right->accept(*this);
        oss << ")";
        result_ = oss.str();
    }
    void visit(const Expression::Grouping& grouping) override
    {
        std::ostringstream oss;
        oss << "(";
        grouping.expression->accept(*this);
        oss << ")";
        result_ = oss.str();
    }
    void visit(const Expression::Literal& literal) override
    {
        std::ostringstream oss;
        std::visit([&oss](const auto& value) { oss << value; }, literal.value);
        result_ = oss.str();
    }
    void visit(const Expression::Unary& unary) override
    {
        std::ostringstream oss;
        oss << "(" << unary.op.lexeme << " ";
        unary.right->accept(*this);
        oss << ")";
        result_ = oss.str();
    }
private:
    std::string result_{};
};


TEST_CASE("visitor")
{
    SECTION("expression")
    {
        std::unique_ptr<Expression> expression = std::make_unique<Expression::Binary>(
            std::make_unique<Expression::Unary>(
                Token{Token::Type::MINUS, "-", Token::Literal{0.84}, Token::Position{0, 0, 0, 0}},
                std::make_unique<Expression::Literal>(149.84)
            ),
            Token{Token::Type::STAR, "*", Token::Literal{-.84}, Token::Position{0, 0, 0, 0}},
            std::make_unique<Expression::Grouping>(
                std::make_unique<Expression::Literal>(true)
            )
        );
    }
}