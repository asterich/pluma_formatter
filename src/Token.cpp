#include "Token.h"

Token::Token() {}

Token::Token(std::string _value, TokenType _type)
    : value(_value), tokenType(_type) {}

Token::Token(const Token &other) {
    this->value = other.value;
    this->tokenType = other.tokenType;
}

Token::Token(Token &&other)
    : value(std::move(other.value)), tokenType(other.tokenType) {}