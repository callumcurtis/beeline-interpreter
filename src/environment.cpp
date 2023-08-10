#include <unordered_map>
#include <memory>

#include "environment.hpp"
#include "lexer.hpp"
#include "interpreter.hpp"


class Environment::Impl
{
public:
    void nested(Impl* parent) {
        parent_ = parent;
    }
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
        if (values_.contains(name))
        {
            values_[name] = value;
        }
        else if (parent_)
        {
            parent_->assign(name, value, position);
        }
        else
        {
            panic("variable '" + name + "' is undefined", position);
        }
    }
    Token::Literal get(const std::string& name, const Token::Position& position) const
    {
        Token::Literal value;
        if (values_.contains(name))
        {
            value = values_.at(name);
        }
        else if (parent_)
        {
            value = parent_->get(name, position);
        }
        else
        {
            panic("variable '" + name + "' is undefined", position);
        }
        return value;
    }
private:
    std::unordered_map<std::string, Token::Literal> values_;
    Impl* parent_;
    void panic(const std::string& message, const Token::Position& position) const
    {
        throw BeelineRuntimeError(message, position);
    }
};


Environment::Environment() : impl_{std::make_unique<Impl>()} {}
Environment::~Environment() = default;
Environment::Environment(const Environment& other) : impl_{std::make_unique<Impl>(*other.impl_)} {}
Environment::Environment(Environment&& other)
{
    impl_ = std::move(other.impl_);
}
Environment& Environment::operator=(const Environment& other)
{
    *impl_ = *other.impl_;
    return *this;
}
Environment& Environment::operator=(Environment&& other)
{
    impl_ = std::move(other.impl_);
    return *this;
}
Environment& Environment::nested(Environment& parent)
{
    impl_->nested(parent.impl_.get());
    return *this;
}
void Environment::define(const std::string& name, const Token::Literal& value, const Token::Position& position)
{
    impl_->define(name, value, position);
}
void Environment::assign(const std::string& name, const Token::Literal& value, const Token::Position& position) {
    impl_->assign(name, value, position);
}
Token::Literal Environment::get(const std::string& name, const Token::Position& position) const {
    return impl_->get(name, position);
}
