#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "lexer.hpp"


TEST_CASE("scan")
{
    SECTION("empty")
    {
        const auto input = "";
        auto lexer = Lexer{input};
        const auto tokens = lexer.scan();
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0].type == Token::Type::END_OF_FILE);
    }
    SECTION("whitespace")
    {
        const auto input = " \t\r";
        auto lexer = Lexer{input};
        const auto tokens = lexer.scan();
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0].type == Token::Type::END_OF_FILE);
    }
    SECTION("comment")
    {
        const auto input = "// comment";
        auto lexer = Lexer{input};
        const auto tokens = lexer.scan();
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0].type == Token::Type::END_OF_FILE);
    }
    SECTION("missing digit after decimal point")
    {
        const auto input = ".h";
        auto lexer = Lexer{input};
        REQUIRE_THROWS_AS(lexer.scan(), BeelineSyntaxError);
    }
    SECTION("unterminated string")
    {
        const auto input = "\"";
        auto lexer = Lexer{input};
        REQUIRE_THROWS_AS(lexer.scan(), BeelineSyntaxError);
    }
    SECTION("unexpected character")
    {
        const auto input = "$";
        auto lexer = Lexer{input};
        REQUIRE_THROWS_AS(lexer.scan(), BeelineSyntaxError);
    }
    SECTION("identifier")
    {
        const auto input = "foobar";
        auto lexer = Lexer{input};
        const auto tokens = lexer.scan();
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == Token::Type::IDENTIFIER);
        REQUIRE(tokens[0].lexeme == "foobar");
        REQUIRE(tokens[1].type == Token::Type::END_OF_FILE);
        REQUIRE(tokens[0].position.offset == 0);
        REQUIRE(tokens[0].position.line == 1);
        REQUIRE(tokens[0].position.column == 1);
        REQUIRE(tokens[0].position.length == 6);
    }
    SECTION("string literal")
    {
        const auto input = "\"foobar\"";
        auto lexer = Lexer{input};
        const auto tokens = lexer.scan();
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].type == Token::Type::STRING);
        REQUIRE(tokens[0].lexeme == "\"foobar\"");
        REQUIRE(tokens[1].type == Token::Type::END_OF_FILE);
        REQUIRE(tokens[0].position.offset == 0);
        REQUIRE(tokens[0].position.line == 1);
        REQUIRE(tokens[0].position.column == 1);
        REQUIRE(tokens[0].position.length == 8);
    }
}
