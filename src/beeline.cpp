#include "beeline.hpp"
#include "lexer.hpp"
#include "logging.hpp"
#include "parser.hpp"
#include "ast/ast.hpp"
#include "ast/visitor.hpp"


BeelineError::BeelineError(const std::string& message) : std::runtime_error(message) {}


std::ostream& operator<<(std::ostream& os, const BeelineError& be)
{
    return os << "BeelineError: " << be.what();
}


void Beeline::run(const std::string& input)
{
    std::vector<Token> tokens = Lexer{input}.scan();

    for(const Token& token : tokens)
    {
        // TODO: Remove this logging.
        log(LoggingLevel::DEBUG) << token;
    }

    std::unique_ptr<Expression> expr = Parser{tokens}.parse();

    if (expr == nullptr)
    {
        return;
    }

    // TODO: Remove this logging.
    ExpressionToString visitor;
    expr->accept(visitor);
    log(LoggingLevel::DEBUG) << visitor.str();
}
