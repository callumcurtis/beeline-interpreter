#include <memory>

#include "interpreter.hpp"
#include "ast/ast.hpp"


class Interpreter::Impl : public Expression::Visitor
{
public:
    void visit(const Expression::Binary& binary) override
    {

    }
    void visit(const Expression::Grouping& grouping) override
    {

    }
    void visit(const Expression::Literal& literal) override
    {

    }
    void visit(const Expression::Unary& unary) override
    {

    }
};
