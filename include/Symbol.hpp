#ifndef SYMBOL_HPP_
#define SYMBOL_HPP_

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace pluma {

// The type of a token.
enum TokenType {
    // EOF token.
    TK_EOF = -1,

    // Nil token.
    NIL,

    // Unknown token.
    UNKNOWN,

    // Error.
    ERROR,

    // Number constant.
    INT_CONST,
    FLOAT_CONST,

    // Identifier.
    IDENTIFIER,

    // C-style string constant.
    STRING_CONST,

    // Character constant.
    CHAR_CONST,

    // Preprocessing.
    SHARP,    // "#"
    INCLUDE,  // "include"
    DEFINE,   // "define"

    // Comments.
    BLOCK_COMMENT,  // "//"
    LINE_COMMENT,   // "/* */"

    // Arithmetic operators.
    ADD,  // "+"
    SUB,  // "-"
    MUL,  // "*"
    DIV,  // "/"
    MOD,  // "%"

    // Bitwise operators.
    BITAND,  // "&"
    BITOR,   // "|"
    BITXOR,  // "^"
    BITNOT,  // "~"
    LSHIFT,  // "<<"
    RSHIFT,  // ">>"

    // Logical operators.
    AND,  // "&&"
    OR,   // "||"
    NOT,  // "!"

    // Comparison operators.
    EQ,   // "=="
    NEQ,  // "!="
    LE,   // "<="
    GE,   // ">="
    LT,   // "<"
    GT,   // ">"

    // Assignment operators.
    ASSIGN,         // "="
    ADD_ASSIGN,     // "+="
    SUB_ASSIGN,     // "-="
    MUL_ASSIGN,     // "*="
    DIV_ASSIGN,     // "/="
    MOD_ASSIGN,     // "%="
    BITAND_ASSIGN,  // "&="
    BITOR_ASSIGN,   // "|="
    BITXOR_ASSIGN,  // "^="
    LSHIFT_ASSIGN,  // "<<="
    RSHIFT_ASSIGN,  // ">>="

    // Increment/Decrement operators.
    INCR,  // "++"
    DECR,  // "--"

    // Brackets.
    LPAREN,     // "("
    RPAREN,     // ")"
    LSBRACKET,  // "["
    RSBRACKET,  // "]"
    LBRACE,     // "{"
    RBRACE,     // "}"

    // Other operators.
    COMMA,          // ","
    PERIOD,         // "."
    ARROW,          // "->"
    COLON,          // ":"
    DBL_COLON,      // "::"
    SEMICOLON,      // ";"
    QUESTION_MARK,  // "?"

    // Keywords.

    // Type specifier.
    CHAR,
    SHORT,
    INT,
    LONG,
    UNSIGNED,
    SIGNED,
    FLOAT,
    DOUBLE,
    VOID,

    STRUCT,
    ENUM,
    UNION,

    // Type qualifier.
    CONST,
    VOLATILE,

    // Storage-class specifier.
    STATIC,
    EXTERN,
    TYPEDEF,

    SIZEOF,

    IF,
    ELSE,
    WHILE,
    DO,
    FOR,
    SWITCH,
    CASE,
    DEFAULT,
    CONTINUE,
    BREAK,
    RETURN,
    GOTO,
};

/**
 * @brief Keyword map using `std::map`.
 * TODO: To be replaced with my implementation.
 */
const std::map<std::string, TokenType> keywordMap{
    // Keyword for basic type
    {"int", TokenType::INT},
    {"char", TokenType::CHAR},
    {"short", TokenType::SHORT},
    {"long", TokenType::LONG},
    {"unsigned", TokenType::UNSIGNED},
    {"signed", TokenType::SIGNED},
    {"float", TokenType::FLOAT},
    {"double", TokenType::INT},
    {"void", TokenType::VOID},

    // Keyword for compound type and enum
    {"struct", TokenType::STRUCT},
    {"enum", TokenType::ENUM},
    {"union", TokenType::UNION},

    // Typedef
    {"typedef", TokenType::TYPEDEF},

    // Type qualifiers
    {"const", TokenType::CONST},
    {"static", TokenType::STATIC},
    {"volatile", TokenType::VOLATILE},
    {"extern", TokenType::EXTERN},

    // Sizeof
    {"sizeof", TokenType::SIZEOF},

    // Control flow
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"do", TokenType::DO},
    {"for", TokenType::FOR},
    {"switch", TokenType::SWITCH},
    {"case", TokenType::CASE},
    {"default", TokenType::DEFAULT},
    {"continue", TokenType::CONTINUE},
    {"break", TokenType::BREAK},
    {"return", TokenType::RETURN},
    {"goto", TokenType::GOTO},
};

/**
 * @brief Preprocessing directive map using `std::map`.
 * TODO: To be replaced with my implementation.
 */
const std::map<std::string, TokenType> preprocessorMap{
    {"include", TokenType::INCLUDE},
    {"define", TokenType::DEFINE},
};

struct Token {
    std::string value;
    TokenType tokenType;

    size_t line;

    Token();
    Token(std::string _value, TokenType _type, size_t _line = 0);
    Token(const Token &other);
    Token(Token &&other);
    ~Token() = default;

    // copy operator required by std::variant
    Token &operator=(const Token &other);
};

// 终结符 / 非终结符
struct Terminal {
    Token token;

    Terminal(const Token &token) : token(token) {}

    // Only for std::map and std::set.
    bool operator<(const Terminal &rhs) const {
        return this->token.tokenType < rhs.token.tokenType;
    }

    bool operator==(const Terminal &rhs) const {
        return this->token.tokenType == rhs.token.tokenType;
    }

    friend std::ostream &operator<<(std::ostream &os, const Terminal &terminal) {
        os << terminal.token.value;
        return os;
    }
};

struct Nonterminal {
    std::string token;

    Nonterminal() : token("") {}

    Nonterminal(const std::string &token) : token(token) {}

    bool operator<(const Nonterminal &rhs) const { return this->token < rhs.token; }

    bool operator==(const Nonterminal &rhs) const { return this->token == rhs.token; }

    friend std::ostream &operator<<(std::ostream &os, const Nonterminal &nonterminal) {
        os << nonterminal.token;
        return os;
    }
};

// 符号
using Sym = std::variant<Terminal, Nonterminal>;

std::ostream &operator<<(std::ostream &os, const Sym &sym);

// 产生式
using SymList = std::vector<Sym>;
using Rule = std::pair<Nonterminal, SymList>;

Rule operator>>(Nonterminal sym, const SymList &symlist);

std::ostream &operator<<(std::ostream &os, const Rule &rule);

struct SymLess {
    constexpr bool operator()(const Sym &lhs, const Sym &rhs) const {
        if (std::holds_alternative<Terminal>(lhs) && std::holds_alternative<Terminal>(rhs)) {
            // both Terminal
            return std::get<Terminal>(lhs) < std::get<Terminal>(rhs);
        } else if (std::holds_alternative<Nonterminal>(lhs) &&
                   std::holds_alternative<Nonterminal>(rhs)) {
            // both Nonterminal
            return std::get<Nonterminal>(lhs) < std::get<Nonterminal>(rhs);
        } else {
            // Terminal < Nonterminal
            return std::holds_alternative<Terminal>(lhs);
        }
    }
};

}  // namespace pluma

#endif