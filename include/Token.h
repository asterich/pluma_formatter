#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>
#include <vector>

// The type of a token.
enum TokenType {
    // Unknown token.
    UNKNOWN,

    // Number.
    NUMBER,

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
    TYPEDEF,

    CONST,
    STATIC,
    VOLATILE,
    EXTERN,

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

class Token {
   public:
    std::string value;
    TokenType tokenType;

   public:
    Token();
    Token(std::string _value, TokenType _type);
    Token(const Token &other);
    Token(Token &&other);
};

#endif