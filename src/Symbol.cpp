#include "Symbol.hpp"

namespace pluma {

Token::Token() : value(std::string("")), tokenType(TokenType::UNKNOWN) {}

Token::Token(std::string _value, TokenType _type, size_t _line, std::vector<Token> _comments)
    : value(_value), tokenType(_type), line(_line), comments(_comments) {}

Token::Token(const Token &other) {
    this->value = other.value;
    this->tokenType = other.tokenType;
    this->line = other.line;
    this->comments = other.comments;
}

Token::Token(Token &&other)
    : value(std::move(other.value)),
      tokenType(other.tokenType),
      line(other.line),
      comments(std::move(other.comments)) {}

Token &Token::operator=(const Token &other) {
    if (this == &other) {
        return *this;
    }
    this->value = other.value;
    this->tokenType = other.tokenType;
    this->line = other.line;
    this->comments = other.comments;
    return *this;
}

Token &Token::operator=(Token &&other) {
    if (this == &other) {
        return *this;
    }
    this->value = std::move(other.value);
    this->tokenType = other.tokenType;
    this->line = other.line;
    this->comments = std::move(other.comments);
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Sym &sym) {
    if (std::holds_alternative<Nonterminal>(sym)) {
        os << std::get<Nonterminal>(sym);
    } else {
        os << std::get<Terminal>(sym);
    }
    return os;
}
Rule operator>>(Nonterminal sym, const SymList &symlist) { return std::make_pair(sym, symlist); }
std::ostream &operator<<(std::ostream &os, const Rule &rule) {
    os << rule.first << " -> ";
    for (size_t i = 0; i < rule.second.size(); ++i) {
        os << " " << rule.second[i] << " ";
    }
    os << std::endl;
    return os;
}

}  // namespace pluma