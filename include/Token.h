#ifndef TOKEN_H_
#define TOKEN_H_

#include <map>
#include <string>
#include <vector>

// The type of a token.
enum TokenType {
    // Unknown token.
    UNKNOWN,

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

class Token {
   public:
    std::string value;
    TokenType tokenType;

   public:
    Token();
    Token(std::string _value, TokenType _type);
    Token(const Token &other);
    Token(Token &&other);
    ~Token() = default;
};

#endif