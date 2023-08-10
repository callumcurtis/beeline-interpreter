#include <memory>
#include <cassert>
#include <variant>
#include <cstddef>

#include "beeline.hpp"
#include "lexer.hpp"
#include "ast/ast.hpp"
#include "interpreter.hpp"


class Interpreter::Impl : public Expression::Visitor
{
public:
    void visit(const Expression::Binary& binary) override
    {
        binary.left->accept(*this);
        Token::Literal left = value_;

        binary.right->accept(*this);
        Token::Literal right = value_;

        switch (binary.op.type)
        {
        case Token::Type::MINUS:
            require<double>(left, binary.op, "left operand must be a number");
            require<double>(right, binary.op, "right operand must be a number");
            value_ = std::get<double>(left) - std::get<double>(right);
            break;
        case Token::Type::SLASH:
            // TODO: Handle division by zero.
            require<double>(left, binary.op, "left operand must be a number");
            require<double>(right, binary.op, "right operand must be a number");
            value_ = std::get<double>(left) / std::get<double>(right);
            break;
        case Token::Type::STAR:
            require<double>(left, binary.op, "left operand must be a number");
            require<double>(right, binary.op, "right operand must be a number");
            value_ = std::get<double>(left) * std::get<double>(right);
            break;
        case Token::Type::PLUS:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            {
                value_ = std::get<double>(left) + std::get<double>(right);
            }
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
            {
                value_ = std::get<std::string>(left) + std::get<std::string>(right);
            }
            else
            {
                panic(binary.op, "operands must be two numbers or two strings");
            }
            break;
        case Token::Type::GREATER:
            require<double>(left, binary.op, "left operand must be a number");
            require<double>(right, binary.op, "right operand must be a number");
            value_ = std::get<double>(left) > std::get<double>(right);
            break;
        case Token::Type::GREATER_EQUAL:
            require<double>(left, binary.op, "left operand must be a number");
            require<double>(right, binary.op, "right operand must be a number");
            value_ = std::get<double>(left) >= std::get<double>(right);
            break;
        case Token::Type::LESS:
            require<double>(left, binary.op, "left operand must be a number");
            require<double>(right, binary.op, "right operand must be a number");
            value_ = std::get<double>(left) < std::get<double>(right);
            break;
        case Token::Type::LESS_EQUAL:
            require<double>(left, binary.op, "left operand must be a number");
            require<double>(right, binary.op, "right operand must be a number");
            value_ = std::get<double>(left) <= std::get<double>(right);
            break;
        case Token::Type::BANG_EQUAL:
            value_ = left != right;
            break;
        case Token::Type::EQUAL_EQUAL:
            value_ = left == right;
            break;
        default:
            assert(false && "unhandled binary operator");
        }
    }
    void visit(const Expression::Grouping& grouping) override
    {
        grouping.expression->accept(*this);
    }
    void visit(const Expression::Literal& literal) override
    {
        value_ = literal.value;
    }
    void visit(const Expression::Unary& unary) override
    {
        unary.right->accept(*this);
        Token::Literal right = value_;

        switch (unary.op.type)
        {
        case Token::Type::MINUS:
            require<double>(right, unary.op, "operand must be a number");
            value_ = -(std::get<double>(right));
            break;
        case Token::Type::BANG:
            require<bool>(right, unary.op, "operand must be a boolean");
            value_ = !(std::get<bool>(right));
            break;
        default:
            assert(false && "unhandled unary operator");
        }
    }
private:
    Token::Literal value_;
    void panic(const Token& token, const std::string& message) const
    {
        throw BeelineRuntimeError(message, token);
    }
    template <typename T>
    void require(const Token::Literal& value, const Token& token, const std::string& message) const
    {
        if (!std::holds_alternative<T>(value))
        {
            panic(token, message);
        }
    }
};


Interpreter::Interpreter() : impl_{std::make_unique<Impl>()} {}
Interpreter::~Interpreter() = default;
void Interpreter::interpret(std::unique_ptr<Expression> expression)
{
    expression->accept(*impl_);
}


BeelineRuntimeError::BeelineRuntimeError(const std::string& message, const Token& token) : BeelineError{message}, token{token} {}


std::ostream& operator<<(std::ostream& os, const BeelineRuntimeError& bre)
{
    return os << "BeelineRuntimeError: " << bre.what() << " at " << bre.token.position;
}
