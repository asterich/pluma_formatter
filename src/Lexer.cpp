#include "Lexer.h"

// Get a char; if the next character is the expected one, return it;
// else unget and return 0.
char expectChar(std::fstream &file, char expectedChar) {
    char actualChar = file.get();
    if (actualChar == expectedChar) {
        return actualChar;
    } else if (actualChar == -1) {
        return -1;
    } else {
        file.unget();
        return 0;
    }
}

Token Lexer::scan(std::fstream &file) {
    if (!file.is_open()) {
        std::cerr << "File is not open.\n";
        return Token();
    }
    if (file.eof()) {
        return Token();
    }

    while (!file.eof()) {
        // Skip blank character.
        for (; isblank(peek) || peek == '\n'; peek = file.get()) {
        }
        if (file.eof() || peek == -1) {
            return Token();
        }

        // TODO: Handle operators.
        // FIXME: Single-character operators at the end of file causes infinite
        // loops.
        {
            std::string value = "";
            switch (peek) {
                case '+': {
                    value.push_back(peek);
                    peek = expectChar(file, '+');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::INCR);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::ADD_ASSIGN);
                    }
                    peek = file.get();
                    return Token(value, TokenType::ADD);
                    break;
                }
                case '-': {
                    value.push_back(peek);
                    peek = expectChar(file, '-');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::DECR);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::SUB_ASSIGN);
                    }
                    peek = file.get();
                    return Token(value, TokenType::SUB);
                    break;
                }
                case '*': {
                    value.push_back(peek);
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::MUL_ASSIGN);
                    }
                    peek = file.get();
                    return Token(value, TokenType::MUL);
                    break;
                }
                case '/': {
                    // TODO: 除了算术运算之外，还有注释需要处理
                    break;
                }
                case '%': {
                    value.push_back(peek);
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::MOD_ASSIGN);
                    }
                    peek = file.get();
                    return Token(value, TokenType::MOD);
                    break;
                }
                case '&': {
                    value.push_back(peek);
                    peek = expectChar(file, '&');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::AND);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::BITAND_ASSIGN);
                    }
                    peek = file.get();
                    return Token(value, TokenType::BITAND);
                    break;
                }
                case '|': {
                    value.push_back(peek);
                    peek = expectChar(file, '|');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::OR);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::BITOR_ASSIGN);
                    }
                    peek = file.get();
                    return Token(value, TokenType::BITOR);
                    break;
                }
                case '^': {
                    value.push_back(peek);
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::BITXOR_ASSIGN);
                    }
                    peek = file.get();
                    return Token(value, TokenType::BITXOR);
                    break;
                }
                case '<': {
                    value.push_back(peek);
                    peek = expectChar(file, '<');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = expectChar(file, '=');
                        if (peek && peek != -1) {
                            value.push_back(peek);
                            peek = file.get();
                            return Token(value, TokenType::LSHIFT_ASSIGN);
                        }
                        peek = file.get();
                        return Token(value, TokenType::LSHIFT);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::LE);
                    }
                    peek = file.get();
                    return Token(value, TokenType::LT);
                }
                case '>': {
                    value.push_back(peek);
                    peek = expectChar(file, '>');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = expectChar(file, '=');
                        if (peek && peek != -1) {
                            value.push_back(peek);
                            peek = file.get();
                            return Token(value, TokenType::RSHIFT_ASSIGN);
                        }
                        peek = file.get();
                        return Token(value, TokenType::RSHIFT);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::GE);
                    }
                    peek = file.get();
                    return Token(value, TokenType::GT);
                }
                case '=': {
                    value.push_back(peek);
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::EQ);
                    }
                    peek = file.get();
                    return Token(value, TokenType::ASSIGN);
                    break;
                }
                default:
                    break;
            }
        }

        // Handle identifiers.
        if (isalpha(peek) || peek == '_') {
            std::string value = "";

            do {
                value.push_back(peek);
                peek = file.get();
            } while (isalpha(peek) || isdigit(peek) || peek == '_');

            return Token(value, TokenType::IDENTIFIER);
        }

        // Handle numbers.
        if (isdigit(peek) || peek == '.') {
            std::string value = "";
            bool isFloat = (peek == '.') ? true : false;
            bool isHex = false;

            value.push_back(peek);

            while ((peek = file.get())) {
                if (isdigit(peek)) {
                } else if (peek == '.') {
                    if (isFloat) {
                        panic("Character \'.\' appears twice in a float.\n");
                    }
                    isFloat = true;
                } else if (peek == 'x' || peek == 'X') {
                    if (value.length() != 1 || value[0] != '0') {
                        panic("Wrong character appeared in a number\n");
                    }
                    isHex = true;
                } else if (isalpha(peek)) {
                    if (!isHex && !((peek >= 'a' && peek <= 'f') ||
                                    (peek >= 'A' && peek <= 'F'))) {
                        panic("Wrong character appeared in a number\n");
                    }
                } else {
                    break;
                }
                value.push_back(peek);
            }

            return Token(value, TokenType::NUMBER);
        }
    }
    return Token();
}