#include <unordered_map>
#include <cctype>
#include <sstream>
#include <cassert>

#include "lexer.hpp"
#include "logging.hpp"


constexpr char EOT = '\x04';


BeelineSyntaxError::BeelineSyntaxError(
    const std::string& message,
    const Token::Position& position
) : BeelineError(message), position_(position) {}


const Token::Position& BeelineSyntaxError::position() const noexcept
{
    return position_;
}


BeelineLexingError::BeelineLexingError(const std::string& message) : BeelineError(message) {}


std::ostream& operator<<(std::ostream& os, const BeelineSyntaxError& bse)
{
    return os << "BeelineSyntaxError: " << bse.what() << " at " << bse.position();
}


std::ostream& operator<<(std::ostream& os, const BeelineLexingError& ble)
{
    return os << "BeelineLexingError: " << ble.what();
}


std::ostream& operator<<(std::ostream& os, const Token::Type& type)
{
    switch (type)
    {
        case Token::Type::LEFT_PARENTHESIS: return os << "LEFT_PARENTHESIS";
        case Token::Type::RIGHT_PARENTHESIS: return os << "RIGHT_PARENTHESIS";
        case Token::Type::LEFT_BRACE: return os << "LEFT_BRACE";
        case Token::Type::RIGHT_BRACE: return os << "RIGHT_BRACE";
        case Token::Type::MINUS: return os << "MINUS";
        case Token::Type::PLUS: return os << "PLUS";
        case Token::Type::SLASH: return os << "SLASH";
        case Token::Type::STAR: return os << "STAR";
        case Token::Type::BANG: return os << "BANG";
        case Token::Type::BANG_EQUAL: return os << "BANG_EQUAL";
        case Token::Type::EQUAL: return os << "EQUAL";
        case Token::Type::EQUAL_EQUAL: return os << "EQUAL_EQUAL";
        case Token::Type::GREATER: return os << "GREATER";
        case Token::Type::GREATER_EQUAL: return os << "GREATER_EQUAL";
        case Token::Type::LESS: return os << "LESS";
        case Token::Type::LESS_EQUAL: return os << "LESS_EQUAL";
        case Token::Type::IDENTIFIER: return os << "IDENTIFIER";
        case Token::Type::STRING: return os << "STRING";
        case Token::Type::NUMBER: return os << "NUMBER";
        case Token::Type::AND: return os << "AND";
        case Token::Type::OR: return os << "OR";
        case Token::Type::IF: return os << "IF";
        case Token::Type::ELSE: return os << "ELSE";
        case Token::Type::TRUE: return os << "TRUE";
        case Token::Type::FALSE: return os << "FALSE";
        case Token::Type::NIL: return os << "NULL";
        case Token::Type::PRINT : return os << "PRINT";
        case Token::Type::VAR: return os << "VAR";
        case Token::Type::WHILE: return os << "WHILE";
        case Token::Type::END_OF_FILE: return os << "END_OF_FILE";
    }
    assert(false && "unhandled token type");
}


std::ostream& operator<<(std::ostream& os, const Token::Position& position)
{
    return os << position.line << ":" << position.column << "-" << (position.column + position.length - 1);
}


std::ostream& operator<<(std::ostream& os, const Token::Literal& literal)
{
    auto visitor = [](const auto& v) -> std::string {
        using V = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<V, bool>) {
            return v ? "true" : "false";
        }
        if constexpr (std::is_same_v<V, double>) {
            return std::to_string(v);
        }
        if constexpr (std::is_same_v<V, std::nullptr_t>) {
            return "nullptr";
        }
        if constexpr (std::is_same_v<V, std::string>) {
            return v;
        }
        assert(false && "unhandled literal type");
    };

    return os << std::visit(visitor, literal);
}


std::ostream& operator<<(std::ostream& os, const Token& token)
{
    return os << token.type << " " << token.lexeme << " " << token.literal << " " << token.position;
}


class Lexer::Impl
{
public:
    Impl(const std::string& input) : input_(input) {}
    const std::vector<Token>& scan()
    {
        bool syntax_error{false};
        std::ostringstream syntax_error_messages;
        while(!is_done())
        {
            try
            {
                scan_remaining_tokens();
            }
            catch (const BeelineSyntaxError& bse)
            {
                syntax_error = true;
                log(LoggingLevel::ERROR) << bse;
                syntax_error_messages << bse << "\n";
            }
        }
        add_token(Token::Type::END_OF_FILE);
        if (syntax_error)
        {
            throw BeelineLexingError(syntax_error_messages.str());
        }
        return tokens_;
    }
private:
    static const std::unordered_map<std::string, Token::Type> keyword_to_type_;
    const std::string input_;
    std::vector<Token> tokens_{};
    std::size_t current_offset_{0};
    std::size_t starting_offset_of_current_token_{0};
    std::size_t current_line_{1};
    std::size_t starting_line_of_current_token_{1};
    std::size_t current_column_{1};
    std::size_t starting_column_of_current_token_{1};
    void scan_remaining_tokens()
    {
        while(!is_done())
        {
            starting_offset_of_current_token_ = current_offset_;
            starting_line_of_current_token_ = current_line_;
            starting_column_of_current_token_ = current_column_;
            scan_next_token();
        }
    }
    void scan_next_token()
    {
        char c = advance();
        switch (c)
        {
            case '(': add_token(Token::Type::LEFT_PARENTHESIS); break;
            case ')': add_token(Token::Type::RIGHT_PARENTHESIS); break;
            case '{': add_token(Token::Type::LEFT_BRACE); break;
            case '}': add_token(Token::Type::RIGHT_BRACE); break;
            case '-': add_token(Token::Type::MINUS); break;
            case '+': add_token(Token::Type::PLUS); break;
            case '*': add_token(Token::Type::STAR); break;
            case '!': add_token(try_consume_match('=') ? Token::Type::BANG_EQUAL : Token::Type::BANG); break;
            case '=': add_token(try_consume_match('=') ? Token::Type::EQUAL_EQUAL : Token::Type::EQUAL); break;
            case '<': add_token(try_consume_match('=') ? Token::Type::LESS_EQUAL : Token::Type::LESS); break;
            case '>': add_token(try_consume_match('=') ? Token::Type::GREATER_EQUAL : Token::Type::GREATER); break;
            case '.':
                if (!std::isdigit(peek()))
                {
                    throw BeelineSyntaxError("missing digit after decimal point", current_token_position());
                }
                number_after_decimal_point();
                break;
            case '/':
                if (try_consume_match('/'))
                {
                    while (peek() != '\n' && !is_done())
                    {
                        advance();
                    }
                }
                else
                {
                    add_token(Token::Type::SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
            case '\n':
                break;
            case '"': string(); break;
            default:
                if (std::isdigit(c))
                {
                    number();
                }
                else if (std::isalpha(c) || c == '_')
                {
                    identifier();
                }
                else
                {
                    throw BeelineSyntaxError("unexpected character", current_token_position());
                }
                break;
        }
    }
    char advance()
    {
        if(peek() == '\n')
        {
            ++current_line_;
            current_column_ = 1;
        }
        else
        {
            ++current_column_;
        }
        return input_[current_offset_++];
    }
    void add_token(const Token::Type type)
    {
        add_token(type, Token::Literal{nullptr});
    }
    void add_token(const Token::Type type, const Token::Literal& literal)
    {
        tokens_.emplace_back(type, current_token_lexeme(), literal, current_token_position());
    }
    bool try_consume_match(const char& expected)
    {
        return try_consume_match(std::string{expected});
    }
    bool try_consume_match(std::string expected)
    {
        if (is_done())
        {
            return false;
        }
        for (std::size_t c{current_offset_}, e{0}; e < expected.size(); ++c, ++e)
        {
            if (is_past_end(c) || input_[c] != expected[e])
            {
                return false;
            }
        }
        for (std::size_t i{0}; i < expected.size(); ++i)
        {
            advance();
        }
        return true;
    }
    char peek() const
    {
        return peek(0);
    }
    char peek(std::size_t ahead) const
    {
        if (is_past_end(current_offset_ + ahead))
        {
            return EOT;
        }
        return input_[current_offset_ + ahead];
    }
    bool is_done() const
    {
        return is_past_end(current_offset_);
    }
    bool is_past_end(std::size_t offset) const
    {
        return offset >= input_.size();
    }
    void string()
    {
        while (peek() != '"' && !is_done())
        {
            advance();
        }
        if (is_done())
        {
            throw BeelineSyntaxError("unterminated string", current_token_position());
        }
        advance();
        const std::string quoted_string_literal{current_token_lexeme()};
        const std::string string_literal = quoted_string_literal.substr(1, quoted_string_literal.size() - 2);
        add_token(Token::Type::STRING, string_literal);
    }
    void number()
    {
        while (std::isdigit(peek()))
        {
            advance();
        }
        if (peek() == '.' && std::isdigit(peek(1)))
        {
            advance();
            return number_after_decimal_point();
        }
        add_token(Token::Type::NUMBER, std::stod(current_token_lexeme()));
    }
    void number_after_decimal_point()
    {
        while (std::isdigit(peek()))
        {
            advance();
        }
        add_token(Token::Type::NUMBER, std::stod(current_token_lexeme()));
    }
    void identifier()
    {
        while (std::isalnum(peek()) || peek() == '_')
        {
            advance();
        }
        auto type_iter = keyword_to_type_.find(current_token_lexeme());
        Token::Type type = Token::Type::IDENTIFIER;
        if (type_iter != keyword_to_type_.end())
        {
            type = type_iter->second;
        }
        add_token(type);
    }
    Token::Position current_token_position() const
    {
        return Token::Position{
            starting_offset_of_current_token_,
            starting_line_of_current_token_,
            starting_column_of_current_token_,
            current_offset_ - starting_offset_of_current_token_,
        };
    }
    std::string current_token_lexeme() const
    {
        Token::Position position = current_token_position();
        return input_.substr(position.offset, position.length);
    }
};


const std::unordered_map<std::string, Token::Type> Lexer::Impl::keyword_to_type_ = {
    {"and", Token::Type::AND},
    {"or", Token::Type::OR},
    {"if", Token::Type::IF},
    {"else", Token::Type::ELSE},
    {"true", Token::Type::TRUE},
    {"false", Token::Type::FALSE},
    {"null", Token::Type::NIL},
    {"print", Token::Type::PRINT},
    {"var", Token::Type::VAR},
    {"while", Token::Type::WHILE},
};


Lexer::Lexer(const std::string& input) : impl_(std::make_unique<Impl>(input)) {}


Lexer::~Lexer() = default;


const std::vector<Token>& Lexer::scan()
{
    return impl_->scan();
}
