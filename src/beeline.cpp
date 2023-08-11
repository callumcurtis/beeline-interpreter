#include "beeline.hpp"
#include "lexer.hpp"
#include "logging.hpp"
#include "parser.hpp"
#include "ast/ast.hpp"
#include "ast/stringify.hpp"
#include "interpreter.hpp"


BeelineError::BeelineError(const std::string& message) : std::runtime_error(message) {}


std::ostream& operator<<(std::ostream& os, const BeelineError& be)
{
    return os << "BeelineError: " << be.what();
}


void Beeline::run(const std::string& input)
{
    try
    {
        std::vector<Token> tokens = Lexer{input}.scan();

        for (const Token& token : tokens)
        {
            log(LoggingLevel::DEBUG) << token;
        }

        std::vector<std::unique_ptr<Statement>> statements = Parser{tokens}.parse();

        for (const std::unique_ptr<Statement>& statement : statements)
        {
            ExpressionToString visitor;
            statement->accept(visitor);
            log(LoggingLevel::DEBUG) << visitor.str();
        }

        Interpreter{}.interpret(std::move(statements));
    }
    catch (const BeelineSyntaxError& bse)
    {
        throw BeelineError{bse.what()};
    }
    catch (const BeelineParseError& bpe)
    {
        throw BeelineError{bpe.what()};
    }
    catch (const BeelineRuntimeError& bre)
    {
        throw BeelineError{bre.what()};
    }
}
