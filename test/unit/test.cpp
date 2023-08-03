#include <catch2/catch.hpp>

#include "lexer.hpp"


TEST_CASE("example")
{
    REQUIRE(Token::Type::LEFT_PARENTHESIS == Token::Type::LEFT_PARENTHESIS);
}