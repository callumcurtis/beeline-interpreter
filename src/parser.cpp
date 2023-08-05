#include <vector>

#include "parser.hpp"
#include "lexer.hpp"


class Parser::Impl
{
public:
    Impl(std::vector<Token> tokens) : tokens_(tokens) {}
private:
    const std::vector<Token> tokens_;
    std::size_t current_token_index_{0};
};
