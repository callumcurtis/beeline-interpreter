# Beeline Grammar

## Syntax Grammar (Parser)

The Beeline language syntax, $B$, is defined using a context-free grammar, $G = (V, \sum, R, S)$, where $V$ is a finite set of variables, $\sum$ is a finite set of terminals, $R$ is a finite set of substitution ruless, and $S \in V$ is the start variable, such that $B = L(G)$.

> $V =$ {program, declaration, variable_declaration, statement, identifier, expression, expression_statement, if_statement, print_statement, while_statement, block, assignment, logical_or, logical_and, equality, comparison, term, factor, unary, end}
> 
> $\sum =$ all ASCII characters
> 
> $R$:
> 
> program $\rightarrow$ declaration* EOF ;
> 
> declaration $\rightarrow$ variable_declaration | statement ;
> 
> variable_declaration $\rightarrow$ "var" identifier | "var" identifier "=" expression end ;
> 
> statement $\rightarrow$ expression_statement | if_statement | print_statement | while_statement | block ;
> 
> expression_statement $\rightarrow$ expression end ;
> 
> if_statement $\rightarrow$ "if" "(" expression ")" statement | "if" "(" expression ")" statement "else" statement ;
> 
> print_statement $\rightarrow$ "print" expression end ;
> 
> while_statement $\rightarrow$ "while" "(" expression ")" statement ;
> 
> block $\rightarrow$ "{" declaration* "}" ;
> 
> expression $\rightarrow$ assignment ;
> 
> assignment $\rightarrow$ identifier "=" assignment | logical_or ;
> 
> logical_or $\rightarrow$ logical_and ("or" logical_and)* ;
> 
> logical_and $\rightarrow$ equality ("and" equality)* ;
> 
> equality $\rightarrow$ comparison (("!=" | "==") comparison)* ;
> 
> comparison $\rightarrow$ term ((">" | ">=" | "<" | "<=") term)* ;
> 
> term $\rightarrow$ factor (("-" | "+") factor)* ;
> 
> factor $\rightarrow$ unary (("/" | "\*") unary)\*;
> 
> unary $\rightarrow$ ("!" | "-") unary ;
>
> end $\rightarrow$ "\\n" | EOF ;
> 
> $S =$ program

## Lexical Grammar (Lexer)

A regular language, $E$, is used by the lexer. A context-free grammar, $H$, with $L(H) = E$, is defined below.

> $V =$ {program, token, number, string, identifier, alpha, digit}
>
> $\sum =$ all ASCII characters
>
> $R:$
>
> program $\rightarrow$ token* EOF ;
>
> token $\rightarrow$ number | string | identifier ;
>
> number $\rightarrow$ digit+ | digit* "." digit+ ;
> 
> string $\rightarrow$ "\\"" <any char except "\\"">* "\\"" ;
> 
> identifier $\rightarrow$ alpha (alpha | digit)* ;
> 
> alpha $\rightarrow$ "a" ... "z" | "A" ... "Z" | "_" ;
> 
> digit $\rightarrow$ "0" ... "9" ;
>
> $S =$ program

## References

[1] “Appendix I · Crafting Interpreters.” https://craftinginterpreters.com/appendix-i.html (accessed Jul. 28, 2023).
