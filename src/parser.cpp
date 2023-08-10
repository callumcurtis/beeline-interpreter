#include <vector>
#include <memory>
#include <initializer_list>
#include <algorithm>
#include <cassert>
#include <string>
#include <optional>

#include "parser.hpp"
#include "ast/ast.hpp"
#include "lexer.hpp"
#include "logging.hpp"


enum struct Associativity
{
    LEFT,
    RIGHT,
};


class Parser::Impl
{
public:
    Impl() = delete;
    Impl(const std::vector<Token>& tokens) : tokens_(tokens) {}
    std::vector<std::unique_ptr<Statement>> parse()
    {
        std::vector<std::unique_ptr<Statement>> statements;
        while (!is_done())
        {
            try
            {
                statements.push_back(declaration());
            }
            catch (const BeelineParseError& e)
            {
                log(LoggingLevel::ERROR) << e;
                recover();
            }
        }
        return statements;
    }
private:
    std::vector<Token> tokens_;
    std::size_t current_token_index_{0};
    void consume_newlines()
    {
        while (is_match(Token::Type::NEWLINE))
        {
            advance();
        }
    }
    bool is_match(const std::initializer_list<Token::Type> types) const
    {
        return std::any_of(
            types.begin(),
            types.end(),
            [this](const Token::Type type) { return is_match(type); }
        );
    }
    bool is_match(const Token::Type type) const
    {
        return (!is_done()) && (peek().type == type);
    }
    bool is_done() const
    {
        return peek().type == Token::Type::END_OF_FILE;
    }
    const Token& peek() const
    {
        return peek(0);
    }
    const Token& peek(const std::size_t ahead) const
    {
        return tokens_.at(current_token_index_ + ahead);
    }
    const Token& advance()
    {
        return tokens_.at(current_token_index_++);
    }
    void require_match(const std::initializer_list<Token::Type> types, const std::string& message)
    {
        if (!is_match(types))
        {
            panic(message);
        }
    }
    void require_match(const Token::Type type, const std::string& message)
    {
        require_match({type}, message);
    }
    void panic(const std::string& message)
    {
        panic(message, peek());
    }
    void panic(const std::string& message, const Token& token)
    {
        throw BeelineParseError(message, token);
    }
    void recover()
    {
        while (!is_done())
        {
            const Token& previous = advance();
            if (previous.type == Token::Type::NEWLINE)
            {
                return;
            }
            switch (peek().type)
            {
                case Token::Type::VAR:
                case Token::Type::IF:
                case Token::Type::WHILE:
                case Token::Type::PRINT:
                    return;
            }
        }
    }
    std::unique_ptr<Expression> binary(
        std::unique_ptr<Expression> (Parser::Impl::*operand)(void),
        const Associativity associativity,
        const std::initializer_list<Token::Type> types
    ) {
        assert(associativity == Associativity::LEFT && "Associativity::RIGHT is not implemented");
        std::unique_ptr<Expression> expr = (this->*operand)();
        while (is_match(types))
        {
            const Token& op = advance();
            std::unique_ptr<Expression> right = std::move((this->*operand)());
            expr = std::make_unique<Expression::Binary>(std::move(expr), op, std::move(right));
        }
        return expr;
    }
    std::unique_ptr<Expression> expression()
    {
        return assignment();
    }
    std::unique_ptr<Expression> assignment()
    {
        std::unique_ptr<Expression> expr = logical_or();
        if (is_match(Token::Type::EQUAL))
        {
            const Token& equals = advance();
            // Finish parsing right-hand side since assignment is right-associative
            std::unique_ptr<Expression> value = assignment();
            if (Expression::Variable* variable = dynamic_cast<Expression::Variable*>(expr.get()))
            {
                return std::make_unique<Expression::Assignment>(variable->name, std::move(value));
            }
            panic("left-hand side of assignment must be a variable", equals);
        }
        return expr;
    }
    std::unique_ptr<Expression> logical_or()
    {
        std::unique_ptr<Expression> expr = logical_and();
        while (is_match(Token::Type::OR))
        {
            const Token& op = advance();
            std::unique_ptr<Expression> right = logical_and();
            expr = std::make_unique<Expression::Binary>(std::move(expr), op, std::move(right));
        }
        return expr;
    }
    std::unique_ptr<Expression> logical_and()
    {
        std::unique_ptr<Expression> expr = equality();
        while (is_match(Token::Type::AND))
        {
            const Token& op = advance();
            std::unique_ptr<Expression> right = equality();
            expr = std::make_unique<Expression::Binary>(std::move(expr), op, std::move(right));
        }
        return expr;
    }
    std::unique_ptr<Expression> equality()
    {
        return binary(&Parser::Impl::comparison, Associativity::LEFT, {Token::Type::BANG_EQUAL, Token::Type::EQUAL_EQUAL});
    }
    std::unique_ptr<Expression> comparison()
    {
        return binary(&Parser::Impl::term, Associativity::LEFT, {Token::Type::GREATER, Token::Type::GREATER_EQUAL, Token::Type::LESS, Token::Type::LESS_EQUAL});
    }
    std::unique_ptr<Expression> term()
    {
        return binary(&Parser::Impl::factor, Associativity::LEFT, {Token::Type::MINUS, Token::Type::PLUS});
    }
    std::unique_ptr<Expression> factor()
    {
        return binary(&Parser::Impl::unary, Associativity::LEFT, {Token::Type::SLASH, Token::Type::STAR});
    }
    std::unique_ptr<Expression> unary()
    {
        if (is_match({Token::Type::BANG, Token::Type::MINUS}))
        {
            const Token& op = advance();
            std::unique_ptr<Expression> right = unary();
            return std::make_unique<Expression::Unary>(op, std::move(right));
        }
        return primary();
    }
    std::unique_ptr<Expression> primary()
    {
        const Token& token = advance();
        std::unique_ptr<Expression> expr;
        switch (token.type)
        {
            case Token::Type::FALSE:
                expr = std::make_unique<Expression::Literal>(Token::Literal{false});
                break;
            case Token::Type::TRUE:
                expr = std::make_unique<Expression::Literal>(Token::Literal{true});
                break;
            case Token::Type::NIL:
                expr = std::make_unique<Expression::Literal>(Token::Literal{nullptr});
                break;
            case Token::Type::NUMBER:
                expr = std::make_unique<Expression::Literal>(token.literal);
                break;
            case Token::Type::STRING:
                expr = std::make_unique<Expression::Literal>(token.literal);
                break;
            case Token::Type::LEFT_PARENTHESIS:
                expr = expression();
                require_match(Token::Type::RIGHT_PARENTHESIS, "expected ')' after expression");
                advance();
                expr = std::make_unique<Expression::Grouping>(std::move(expr));
                break;
            case Token::Type::IDENTIFIER:
                expr = std::make_unique<Expression::Variable>(token);
                break;
            default:
                panic("expected expression", token);
        }
        return expr;
    }
    std::unique_ptr<Statement> declaration()
    {
        consume_newlines();
        std::unique_ptr<Statement> stmt;
        if (is_match(Token::Type::VAR))
        {
            stmt = variable_declaration();
        }
        else
        {
            stmt = statement();
        }
        consume_newlines();
        return stmt;
    }
    std::unique_ptr<Statement> statement()
    {
        std::unique_ptr<Statement> stmt;
        switch (peek().type)
        {
            case Token::Type::PRINT:
                stmt = print_statement();
                break;
            case Token::Type::LEFT_BRACE:
                stmt = block();
                break;
            case Token::Type::IF:
                stmt = if_statement();
                break;
            default:
                stmt = expression_statement();
                break;
        }
        return stmt;
    }
    std::unique_ptr<Statement> print_statement()
    {
        assert(is_match(Token::Type::PRINT));
        const Token& keyword = advance();
        std::unique_ptr<Expression> expr = expression();
        if (!is_done())
        {
            require_match(Token::Type::NEWLINE, "expected newline or EOF after expression");
            advance();
        }
        return std::make_unique<Statement::Print>(keyword, std::move(expr));
    }
    std::unique_ptr<Statement> block()
    {
        assert(is_match(Token::Type::LEFT_BRACE));
        advance();
        std::vector<std::unique_ptr<Statement>> statements;
        while (!is_match(Token::Type::RIGHT_BRACE) && !is_done())
        {
            statements.push_back(declaration());
        }
        require_match(Token::Type::RIGHT_BRACE, "expected '}' after block");
        advance();
        return std::make_unique<Statement::Block>(std::move(statements));
    }
    std::unique_ptr<Statement> if_statement()
    {
        assert(is_match(Token::Type::IF));
        const Token& if_keyword = advance();
        require_match(Token::Type::LEFT_PARENTHESIS, "expected '(' after 'if'");
        advance();
        std::unique_ptr<Expression> condition = expression();
        require_match(Token::Type::RIGHT_PARENTHESIS, "expected ')' after if condition");
        advance();
        std::unique_ptr<Statement> then_statement = statement();
        std::optional<Token> else_keyword;
        std::unique_ptr<Statement> else_statement;
        if (is_match(Token::Type::ELSE))
        {
            else_keyword = advance();
            else_statement = statement();
        }
        return std::make_unique<Statement::IfElse>(std::move(condition), if_keyword, std::move(then_statement), else_keyword, std::move(else_statement));
    }
    std::unique_ptr<Statement> expression_statement()
    {
        std::unique_ptr<Expression> expr = expression();
        if (!is_done())
        {
            require_match(Token::Type::NEWLINE, "expected newline or EOF after expression");
            advance();
        }
        return std::make_unique<Statement::Expression>(std::move(expr));
    }
    std::unique_ptr<Statement> variable_declaration()
    {
        assert(is_match(Token::Type::VAR));
        advance();
        require_match(Token::Type::IDENTIFIER, "expected identifier");
        const Token& name = advance();
        std::unique_ptr<Expression> initializer;
        if (is_match(Token::Type::EQUAL))
        {
            advance();
            initializer = expression();
        }
        if (!is_done())
        {
            require_match({Token::Type::NEWLINE}, "expected newline or EOF after variable declaration");
            advance();
        }
        return std::make_unique<Statement::VariableDeclaration>(name, std::move(initializer));
    }
};


Parser::Parser(const std::vector<Token>& tokens) : impl_(std::make_unique<Impl>(tokens)) {}
Parser::~Parser() = default;
std::vector<std::unique_ptr<Statement>> Parser::parse() { return impl_->parse(); }


BeelineParseError::BeelineParseError(const std::string& message, const Token& token) : BeelineError(message), token(token) {}


std::ostream& operator<<(std::ostream& os, const BeelineParseError& bpe)
{
    return os << "BeelineParseError: " << bpe.what() << " at " << bpe.token.position;
}
