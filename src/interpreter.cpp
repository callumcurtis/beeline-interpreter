#include <memory>
#include <cassert>
#include <variant>
#include <cstddef>
#include <iostream>

#include "beeline.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"
#include "environment.hpp"
#include "replace.hpp"
#include "logging.hpp"


class Interpreter::Impl : public Expression::Visitor, public Statement::Visitor
{
public:
    void visit(const Expression::Binary& binary) override
    {
        Token::Literal left;
        Token::Literal right;

        switch (binary.op.type)
        {
            case Token::Type::AND:
                binary.left->accept(*this);
                left = value_;
                require<bool>(left, binary.op, "left operand must be a boolean");
                // short-circuit evaluation
                if (std::get<bool>(left))
                {
                    binary.right->accept(*this);
                    right = value_;
                    require<bool>(right, binary.op, "right operand must be a boolean");
                }
                break;
            case Token::Type::OR:
                binary.left->accept(*this);
                left = value_;
                require<bool>(left, binary.op, "left operand must be a boolean");
                // short-circuit evaluation
                if (!std::get<bool>(left))
                {
                    binary.right->accept(*this);
                    right = value_;
                    require<bool>(right, binary.op, "right operand must be a boolean");
                }
                break;
            case Token::Type::MINUS:
                binary.left->accept(*this);
                left = value_;
                binary.right->accept(*this);
                right = value_;
                require<double>(left, binary.op, "left operand must be a number");
                require<double>(right, binary.op, "right operand must be a number");
                value_ = std::get<double>(left) - std::get<double>(right);
                break;
            case Token::Type::SLASH:
                binary.left->accept(*this);
                left = value_;
                binary.right->accept(*this);
                right = value_;
                require<double>(left, binary.op, "left operand must be a number");
                require<double>(right, binary.op, "right operand must be a number");
                if (std::get<double>(right) == 0)
                {
                    panic(binary.op, "division by zero");
                }
                value_ = std::get<double>(left) / std::get<double>(right);
                break;
            case Token::Type::STAR:
                binary.left->accept(*this);
                left = value_;
                binary.right->accept(*this);
                right = value_;
                require<double>(left, binary.op, "left operand must be a number");
                require<double>(right, binary.op, "right operand must be a number");
                value_ = std::get<double>(left) * std::get<double>(right);
                break;
            case Token::Type::PLUS:
            {
                binary.left->accept(*this);
                left = value_;
                binary.right->accept(*this);
                right = value_;
                require_not<std::nullptr_t>(left, binary.op, "left operand must not be null");
                require_not<std::nullptr_t>(right, binary.op, "right operand must not be null");
                if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
                {
                    panic(binary.op, "cannot add two booleans");
                }
                const bool is_concatenation = std::holds_alternative<std::string>(left) || std::holds_alternative<std::string>(right);
                if (is_concatenation)
                {
                    to_string(left);
                    to_string(right);
                    value_ = std::get<std::string>(left) + std::get<std::string>(right);
                }
                else
                {
                    require<double>(left, binary.op, "left operand must be a number to participate in addition");
                    require<double>(right, binary.op, "right operand must be a number to participate in addition");
                    value_ = std::get<double>(left) + std::get<double>(right);
                }
                break;
            }
            case Token::Type::GREATER:
                binary.left->accept(*this);
                left = value_;
                binary.right->accept(*this);
                right = value_;
                require<double>(left, binary.op, "left operand must be a number");
                require<double>(right, binary.op, "right operand must be a number");
                value_ = std::get<double>(left) > std::get<double>(right);
                break;
            case Token::Type::GREATER_EQUAL:
                binary.left->accept(*this);
                left = value_;
                binary.right->accept(*this);
                right = value_;
                require<double>(left, binary.op, "left operand must be a number");
                require<double>(right, binary.op, "right operand must be a number");
                value_ = std::get<double>(left) >= std::get<double>(right);
                break;
            case Token::Type::LESS:
                binary.left->accept(*this);
                left = value_;
                binary.right->accept(*this);
                right = value_;
                require<double>(left, binary.op, "left operand must be a number");
                require<double>(right, binary.op, "right operand must be a number");
                value_ = std::get<double>(left) < std::get<double>(right);
                break;
            case Token::Type::LESS_EQUAL:
                binary.left->accept(*this);
                left = value_;
                binary.right->accept(*this);
                right = value_;
                require<double>(left, binary.op, "left operand must be a number");
                require<double>(right, binary.op, "right operand must be a number");
                value_ = std::get<double>(left) <= std::get<double>(right);
                break;
            case Token::Type::BANG_EQUAL:
                binary.left->accept(*this);
                left = value_;
                binary.right->accept(*this);
                right = value_;
                value_ = left != right;
                break;
            case Token::Type::EQUAL_EQUAL:
                binary.left->accept(*this);
                left = value_;
                binary.right->accept(*this);
                right = value_;
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
        switch (unary.op.type)
        {
            case Token::Type::MINUS:
                require<double>(value_, unary.op, "operand must be a number");
                value_ = -(std::get<double>(value_));
                break;
            case Token::Type::BANG:
                require<bool>(value_, unary.op, "operand must be a boolean");
                value_ = !(std::get<bool>(value_));
                break;
            default:
                assert(false && "unhandled unary operator");
        }
    }
    void visit(const Expression::Variable& variable) override
    {
        value_ = environment_.get(variable.name.lexeme, variable.name.position);
    }
    void visit(const Expression::Assignment& assignment) override
    {
        assignment.value->accept(*this);
        environment_.assign(assignment.name.lexeme, value_, assignment.name.position);
    }
    void visit(const Statement::Expression& expression) override
    {
        expression.expression->accept(*this);
    }
    void visit(const Statement::Print& print) override
    {
        print.expression->accept(*this);
        require<std::string>(value_, print.keyword, "operand must be a string");
        std::cout << std::get<std::string>(value_);
    }
    void visit(const Statement::VariableDeclaration& variable_declaration) override
    {
        value_ = nullptr;
        if (variable_declaration.initializer)
        {
            variable_declaration.initializer->accept(*this);
        }
        environment_.define(variable_declaration.name.lexeme, value_, variable_declaration.name.position);
    }
    void visit(const Statement::Block& block) override
    {
        ScopedReplace replacer(environment_, Environment{}.nested(environment_));
        for (const std::unique_ptr<Statement>& statement : block.statements)
        {
            statement->accept(*this);
        }
        value_ = nullptr;
    }
    void visit(const Statement::IfElse& if_else) override
    {
        if_else.condition->accept(*this);
        require<bool>(value_, if_else.if_keyword, "condition must evaluate to a boolean");
        if (std::get<bool>(value_))
        {
            if_else.then_statement->accept(*this);
        }
        else if (if_else.else_statement)
        {
            if_else.else_statement->accept(*this);
        }
        value_ = nullptr;
    }
    void visit(const Statement::WhileLoop& while_loop) override
    {
        auto check_condition = [&]() -> bool
        {
            while_loop.condition->accept(*this);
            require<bool>(value_, while_loop.keyword, "condition must evaluate to a boolean");
            return std::get<bool>(value_);
        };
        while (check_condition())
        {
            while_loop.body->accept(*this);
        }
        value_ = nullptr;
    }
private:
    Token::Literal value_;
    void panic(const Token& token, const std::string& message) const
    {
        BeelineRuntimeError bre{message, token.position};
        log(LoggingLevel::ERROR) << bre;
        throw bre;
    }
    template <typename T>
    void require(const Token::Literal& value, const Token& token, const std::string& message) const
    {
        if (!std::holds_alternative<T>(value))
        {
            panic(token, message);
        }
    }
    template <typename T>
    void require_not(const Token::Literal& value, const Token& token, const std::string& message) const
    {
        if (std::holds_alternative<T>(value))
        {
            panic(token, message);
        }
    }
    void to_string(Token::Literal& value) const
    {
        if (std::holds_alternative<std::string>(value))
        {
            return;
        }
        if (std::holds_alternative<double>(value))
        {
            value = std::to_string(std::get<double>(value));
            std::get<std::string>(value).erase(std::get<std::string>(value).find_last_not_of('0') + 1, std::string::npos);
            std::get<std::string>(value).erase(std::get<std::string>(value).find_last_not_of('.') + 1, std::string::npos);
        }
        else if (std::holds_alternative<bool>(value))
        {
            value = std::get<bool>(value) ? "true" : "false";
        }
        else
        {
            assert(false && "unable to convert to string");
        }
    }
private:
    Environment environment_;
};


Interpreter::Interpreter() : impl_{std::make_unique<Impl>()} {}
Interpreter::~Interpreter() = default;
void Interpreter::interpret(const std::vector<std::unique_ptr<Statement>> statements)
{
    for (const std::unique_ptr<Statement>& statement : statements)
    {
        statement->accept(*impl_);
    }
}


BeelineRuntimeError::BeelineRuntimeError(const std::string& message, const Token::Position& position) : BeelineError{message}, position{position} {}


std::ostream& operator<<(std::ostream& os, const BeelineRuntimeError& bre)
{
    return os << "BeelineRuntimeError: " << bre.what() << " at " << bre.position;
}
