#include "beeline.hpp"
#include "lexer.hpp"
#include "logging.hpp"


BeelineError::BeelineError(const std::string& message) : std::runtime_error(message) {}


std::ostream& operator<<(std::ostream& os, const BeelineError& be)
{
    return os << "BeelineError: " << be.what();
}


void Beeline::run(const std::string& input)
{
    Lexer lexer{input};
    std::vector<Token> tokens = lexer.scan();
    for(const Token& token : tokens)
    {
        // TODO: Remove this logging.
        log(LoggingLevel::DEBUG) << token << " ";
    }
    log(LoggingLevel::DEBUG) << "\n";
}
