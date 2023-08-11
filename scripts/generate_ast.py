"""AST Generator

Generates the AST classes for the parser. Writes to stdout.
Accepts the output programming language as a command line argument.
"""

import abc
import argparse
import re


class Field:
    def __init__(
        self,
        name: str,
        type: str,
    ):
        self.name = name
        self.type = type


class Type:
    def __init__(
        self,
        name: str,
        fields: list[Field],
        parent: str = None,
    ):
        self.name = name
        self.fields = fields
        self.parent = parent


class ABC:
    def __init__(
        self,
        name: str,
        parent: str = None,
    ):
        self.name = name
        self.parent = parent


EXPRESSION = ABC("Expression")
STATEMENT = ABC("Statement")


TYPES = [
    EXPRESSION,
    Type("Binary", [Field("left", "std::unique_ptr<Expression>"), Field("op", "Token"), Field("right", "std::unique_ptr<Expression>")], EXPRESSION),
    Type("Grouping", [Field("expression", "std::unique_ptr<Expression>")], EXPRESSION),
    Type("Literal", [Field("value", "Token::Literal")], EXPRESSION),
    Type("Unary", [Field("op", "Token"), Field("right", "std::unique_ptr<Expression>")], EXPRESSION),
    Type("Variable", [Field("name", "Token")], EXPRESSION),
    Type("Assignment", [Field("name", "Token"), Field("value", "std::unique_ptr<Expression>")], EXPRESSION),
    STATEMENT,
    Type("Expression", [Field("expression", "std::unique_ptr<::Expression>")], STATEMENT),
    Type("Print", [Field("keyword", "Token"), Field("expression", "std::unique_ptr<::Expression>")], STATEMENT),
    Type("VariableDeclaration", [Field("name", "Token"), Field("initializer", "std::unique_ptr<::Expression>")], STATEMENT),
    Type("Block", [Field("statements", "std::vector<std::unique_ptr<Statement>>")], STATEMENT),
    Type("IfElse", [Field("condition", "std::unique_ptr<::Expression>"), Field("if_keyword", "Token"), Field("then_statement", "std::unique_ptr<Statement>"), Field("else_keyword", "std::optional<Token>"), Field("else_statement", "std::unique_ptr<Statement>")], STATEMENT),
    Type("WhileLoop", [Field("keyword", "Token"), Field("condition", "std::unique_ptr<::Expression>"), Field("body", "std::unique_ptr<Statement>")], STATEMENT),
]


def to_snake_case(s: str) -> str:
    # Taken from: https://stackoverflow.com/a/1176023
    return re.sub(r'(?<!^)(?=[A-Z])', '_', s).lower()


class AstMetaProgrammer (abc.ABC):
    def __init__(self, types: list[Type]):
        self._types = types
        self._visitable_types_by_parent = {}
        for t in self._types:
            if isinstance(t, ABC):
                continue
            if t.parent not in self._visitable_types_by_parent:
                self._visitable_types_by_parent[t.parent] = []
            self._visitable_types_by_parent[t.parent].append(t)

    def generate(self) -> str:
        return "".join([
            self.generate_docstring(),
            self.generate_includes(),
            self.generate_ast_classes(),
            self.generate_visitors(),
            "\n"
        ])

    @abc.abstractmethod
    def generate_docstring(self) -> str:
        pass

    @abc.abstractmethod
    def generate_includes(self) -> str:
        pass

    @abc.abstractmethod
    def generate_ast_classes(self) -> str:
        pass

    @abc.abstractmethod
    def generate_visitors(self) -> str:
        pass


class CppAstMetaProgrammer (AstMetaProgrammer):
    def __init__(self, types: list[Type], cli_args):
        super().__init__(types)
        parser = argparse.ArgumentParser()
        parser.add_argument("--header", required=True)
        args = parser.parse_args(cli_args)
        self._header = args.header


    def generate_docstring(self):
        return "/*\n * This file was generated by the generate_ast.py script.\n */"

    def generate_includes(self):
        return "".join([
            "\n\n#include <optional>",
            f"\n\n#include {self._header}",
        ])


    def generate_ast_classes(self):
        return "".join([
            self.generate_ast_class(t)
            for t in self._types
        ])

    def generate_ast_class(self, t: Type):
        return self.generate_ast_class_methods(t)

    def generate_ast_class_methods(self, t: Type):
        if isinstance(t, ABC):
            return ""
        return "".join([
            "\n\n",
            f"\n{t.parent.name}::{t.name}::{t.name}({', '.join (f'{field.type} {field.name}' for field in t.fields)}) : {', '.join (f'{field.name}{{std::move({field.name})}}' for field in t.fields)} {{}}",
            f"\nvoid {t.parent.name}::{t.name}::accept({t.parent.name}::Visitor& visitor) const {{ visitor.visit(*this); }}"
        ])

    def generate_visitors(self):
        return ""


class HppAstMetaProgrammer (AstMetaProgrammer):
    def __init__(self, types: list[Type], cli_args):
        super().__init__(types)

    def generate_docstring(self):
        return "/*\n * This file was generated by the generate_ast.py script.\n */"

    def generate_includes(self):
        return "".join([
            "\n\n#pragma once",
            "\n\n#include <memory>",
            "\n#include <optional>",
            "\n\n#include \"lexer.hpp\"",
        ])

    def generate_ast_classes(self):
        return "".join([
            self.generate_ast_class(t)
            for t in self._types
        ])

    def generate_ast_class(self, t: Type):
        return "".join([
            self.generate_ast_class_header(t),
            self.generate_ast_class_types(t),
            self.generate_ast_class_methods(t),
            self.generate_ast_class_fields(t),
            self.generate_ast_class_footer(t),
        ])

    def generate_ast_class_header(self, t: Type):
        if isinstance(t, ABC):
            return f"\n\n\nstruct {t.name}\n{{"
        return f"\n\n\nstruct {t.parent.name}::{t.name} : {t.parent.name}\n{{"
    
    def generate_ast_class_types(self, t: Type):
        if not isinstance(t, ABC):
            return ""
        return "\n".join([""] + ["    class Visitor;"] + [f"    class {sub_t.name};" for sub_t in self._types if sub_t.parent == t])

    def generate_ast_class_methods(self, t: Type):
        if isinstance(t, ABC):
            return "".join([
                f"\n    virtual ~{t.name}() = default;",
                "\n    virtual void accept(Visitor& visitor) const = 0;",
            ])
        return "".join([
            f"\n    {t.name}({', '.join (f'{field.type} {field.name}' for field in t.fields)});",
            f"\n    void accept({t.parent.name}::Visitor& visitor) const override;",
        ])

    def generate_ast_class_fields(self, t: Type):
        if isinstance(t, ABC):
            return ""
        return "\n".join([""] + [f"    {field.type} {field.name};" for field in t.fields])

    def generate_ast_class_footer(self, t: Type):
        return "\n};"

    def generate_visitors(self):
        return "".join([
            self.generate_visitor(t)
            for t in self._visitable_types_by_parent.keys()
        ])
    
    def generate_visitor(self, t: Type):
        return "".join([
            self.generate_visitor_header(t),
            self.generate_visitor_methods(t),
            self.generate_visitor_footer(t),
        ])

    def generate_visitor_header(self, t: Type):
        return f"\n\n\nclass {t.name}::Visitor\n{{"

    def generate_visitor_methods(self, t: Type):
        return "".join(
            ["\npublic:"]
            + [
                f"\n    virtual void visit(const {v_t.name}& {to_snake_case(v_t.name)}) = 0;"
                if isinstance(v_t, ABC) else
                f"\n    virtual void visit(const {v_t.parent.name}::{v_t.name}& {to_snake_case(v_t.name)}) = 0;"
                for v_t in self._visitable_types_by_parent.get(t)
            ]
        )

    def generate_visitor_footer(self, t: Type):
        return "\n};"


def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate AST classes for the parser.",
    )
    parser.add_argument(
        "language",
        choices=["cpp", "hpp"],
        help="The output programming language.",
    )
    return parser.parse_known_args()


METAPROGRAMMER_BY_LANGUAGE = {
    "cpp": CppAstMetaProgrammer,
    "hpp": HppAstMetaProgrammer,
}


def main():
    known_args, unknown_args = parse_args()
    if known_args.language not in METAPROGRAMMER_BY_LANGUAGE:
        raise ValueError(f"Unknown language: {known_args.language}")
    metaprogrammer = METAPROGRAMMER_BY_LANGUAGE[known_args.language](TYPES, cli_args=unknown_args)
    print(metaprogrammer.generate(), end="")


if __name__ == "__main__":
    main()
