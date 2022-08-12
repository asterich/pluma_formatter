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
        if (file.eof()) {
            return Token();
        }

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
                    value.push_back(peek);

                    // "/="
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::DIV_ASSIGN);
                    }

                    // "//"
                    peek = expectChar(file, '/');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        while ((peek = file.get()) != '\n') {
                            value.push_back(peek);
                        }
                        peek = file.get();
                        return Token(value, TokenType::LINE_COMMENT);
                    }

                    // "/* */"
                    peek = expectChar(file, '*');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        for (peek = file.get(); peek != -1 && peek != '*';
                             peek = file.get()) {
                            value.push_back(peek);
                        }
                        switch (peek) {
                            case -1: {
                                panic("Block comment doesn't close.\n");
                                return Token();
                                break;
                            }
                            case '*': {
                                value.push_back(peek);
                                peek = expectChar(file, '/');
                                if (peek && peek != -1) {
                                    value.push_back(peek);
                                    peek = file.get();
                                    return Token(value,
                                                 TokenType::BLOCK_COMMENT);
                                } else {
                                    panic("Block comment doesn't close.\n");
                                    return Token();
                                    break;
                                }
                            }
                        }
                    }

                    // "/"
                    peek = file.get();
                    return Token(value, TokenType::DIV);
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
                case '~': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::BITNOT);
                }
                case '!': {
                    value.push_back(peek);
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::NEQ);
                    }
                    peek = file.get();
                    return Token(value, TokenType::NOT);
                }

                // Brackets.
                case '(': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::LPAREN);
                }
                case ')': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::RPAREN);
                }
                case '[': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::LSBRACKET);
                }
                case ']': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::RSBRACKET);
                }
                case '{': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::LBRACE);
                }
                case '}': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::RBRACE);
                }

                case ',': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::COMMA);
                }
                case '.': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::PERIOD);
                }
                case ':': {
                    value.push_back(peek);
                    peek = expectChar(file, ':');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::DBL_COLON);
                    }
                    peek = file.get();
                    return Token(value, TokenType::COLON);
                }
                case ';': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::SEMICOLON);
                }
                case '?': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::QUESTION_MARK);
                }

                case '#': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::SHARP);
                }

                default:
                    break;
            }
        }

        // Handle keywords and identifiers.
        if (isalpha(peek) || peek == '_') {
            std::string value = "";

            do {
                value.push_back(peek);
                peek = file.get();
            } while (isalpha(peek) || isdigit(peek) || peek == '_');

            if (auto token_pair = keywordMap.find(value);
                token_pair != keywordMap.end()) {
                return Token(token_pair->first, token_pair->second);
            } else if (auto token_pair = preprocessorMap.find(value);
                       token_pair != preprocessorMap.end()) {
                return Token(token_pair->first, token_pair->second);
            }

            return Token(value, TokenType::IDENTIFIER);
        }

        // Handle C-style string constant.
        if (peek == '\"') {
            std::string value = "";
            value.push_back(peek);
            while ((peek = file.get()) != '\"') {
                if (file.eof() || peek == '\n') {
                    panic("Open string constant!");
                    return Token();
                }
                value.push_back(peek);
            }
            value.push_back(peek);
            peek = file.get();
            return Token(value, TokenType::STRING_CONST);
        }

        // Handle character constant.
        if (peek == '\'') {
            std::string value = "";

            value.push_back(peek);

            peek = expectChar(file, '\\');
            if (peek && peek != -1) {
                value.push_back(peek);
            }
            peek = file.get();
            if (!isascii(peek)) {
                panic("Character constant includes non-ascii character!\n");
                return Token();
            }
            value.push_back(peek);
            peek = expectChar(file, '\'');
            if (!peek) {
                panic(
                    "Quotation mark doesn't close, or there are more than "
                    "1 character in the char constant.\n");
                return Token();
            }
            value.push_back(peek);
            peek = file.get();
            return Token(value, TokenType::CHAR_CONST);
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