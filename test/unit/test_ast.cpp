#include <catch2/catch.hpp>

#include <sstream>

#include "ast.hpp"
#include "stringify.hpp"


TEST_CASE("tostring")
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
        ExpressionToString visitor;
        expression->accept(visitor);
        REQUIRE(visitor.str() == "((- 149.840000) * (true))");
    }
}