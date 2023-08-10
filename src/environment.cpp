#include <unordered_map>
#include <memory>

#include "environment.hpp"
#include "lexer.hpp"
#include "interpreter.hpp"


class Environment::Impl
{
public:
    void define(const std::string& name, const Token::Literal& value, const Token::Position& position)
    {
        if (values_.contains(name))
        {
            panic("variable '" + name + "' is already defined", position);
        }
        values_[name] = value;
    }
    void assign(const std::string& name, const Token::Literal& value, const Token::Position& position)
    {
        if (!values_.contains(name))
        {
            panic("variable '" + name + "' is undefined", position);
        }
        values_[name] = value;
    }
    Token::Literal& get(const std::string& name, const Token::Position& position)
    {
        if (!values_.contains(name))
        {
            panic("variable '" + name + "' is undefined", position);
        }
        return values_.at(name);
    }
private:
    std::unordered_map<std::string, Token::Literal> values_{};
    void panic(const std::string& message, const Token::Position& position) const
    {
        throw BeelineRuntimeError(message, position);
    }
};


Environment::Environment() : impl_{std::make_unique<Impl>()} {}
Environment::~Environment() = default;
void Environment::define(const std::string& name, const Token::Literal& value, const Token::Position& position) { impl_->define(name, value, position); }
void Environment::assign(const std::string& name, const Token::Literal& value, const Token::Position& position) { impl_->assign(name, value, position); }
Token::Literal& Environment::get(const std::string& name, const Token::Position& position) { return impl_->get(name, position); }
