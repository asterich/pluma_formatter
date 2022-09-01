#include "Lexer.h"

namespace pluma {

Lexer::Lexer(std::string inputFilename) {
    this->inputFile.open(inputFilename);

    if (!this->inputFile.is_open()) {
        this->inputFile.clear();
        this->inputFile.open(inputFilename, std::ios::out);  // 创建文件
        this->inputFile.close();
        this->inputFile.open(inputFilename);
    }
}

Lexer::~Lexer() { this->inputFile.close(); }

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
            if (peek == '\n') {
                ++line;
            }
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
                        return Token(value, TokenType::INCR, line);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::ADD_ASSIGN, line);
                    }
                    peek = file.get();
                    return Token(value, TokenType::ADD, line);
                    break;
                }
                case '-': {
                    value.push_back(peek);
                    peek = expectChar(file, '-');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::DECR, line);
                    }
                    peek = expectChar(file, '>');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::ARROW, line);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::SUB_ASSIGN, line);
                    }
                    peek = file.get();
                    return Token(value, TokenType::SUB, line);
                    break;
                }
                case '*': {
                    value.push_back(peek);
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::MUL_ASSIGN, line);
                    }
                    peek = file.get();
                    return Token(value, TokenType::MUL, line);
                    break;
                }
                case '/': {
                    value.push_back(peek);

                    // "/="
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::DIV_ASSIGN, line);
                    }

                    // "//"
                    peek = expectChar(file, '/');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        while ((peek = file.get()) != '\n') {
                            value.push_back(peek);
                        }
                        peek = file.get();
                        return Token(value, TokenType::LINE_COMMENT, line);
                    }

                    // "/* */"
                    peek = expectChar(file, '*');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        for (peek = file.get(); peek != -1 && peek != '*'; peek = file.get()) {
                            value.push_back(peek);
                            if (peek == '\n') {
                                ++line;
                            }
                        }
                        switch (peek) {
                            case -1: {
                                char errmsg[63];
                                sprintf(errmsg,
                                        "At line %d:\n"
                                        "Block comment doesn't close.\n",
                                        line);
                                panic(errmsg);
                                return Token();
                                break;
                            }
                            case '*': {
                                value.push_back(peek);
                                peek = expectChar(file, '/');
                                if (peek && peek != -1) {
                                    value.push_back(peek);
                                    peek = file.get();
                                    return Token(value, TokenType::BLOCK_COMMENT, line);
                                } else {
                                    char errmsg[63];
                                    sprintf(errmsg,
                                            "At line %d:\n"
                                            "Block comment doesn't close.\n",
                                            line);
                                    panic(errmsg);
                                    return Token();
                                    break;
                                }
                            }
                        }
                    }

                    // "/"
                    peek = file.get();
                    return Token(value, TokenType::DIV, line);
                    break;
                }
                case '%': {
                    value.push_back(peek);
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::MOD_ASSIGN, line);
                    }
                    peek = file.get();
                    return Token(value, TokenType::MOD, line);
                    break;
                }
                case '&': {
                    value.push_back(peek);
                    peek = expectChar(file, '&');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::AND, line);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::BITAND_ASSIGN, line);
                    }
                    peek = file.get();
                    return Token(value, TokenType::BITAND, line);
                    break;
                }
                case '|': {
                    value.push_back(peek);
                    peek = expectChar(file, '|');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::OR, line);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::BITOR_ASSIGN, line);
                    }
                    peek = file.get();
                    return Token(value, TokenType::BITOR, line);
                    break;
                }
                case '^': {
                    value.push_back(peek);
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::BITXOR_ASSIGN, line);
                    }
                    peek = file.get();
                    return Token(value, TokenType::BITXOR, line);
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
                            return Token(value, TokenType::LSHIFT_ASSIGN, line);
                        }
                        peek = file.get();
                        return Token(value, TokenType::LSHIFT, line);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::LE, line);
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
                            return Token(value, TokenType::RSHIFT_ASSIGN, line);
                        }
                        peek = file.get();
                        return Token(value, TokenType::RSHIFT, line);
                    }
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::GE, line);
                    }
                    peek = file.get();
                    return Token(value, TokenType::GT, line);
                }
                case '=': {
                    value.push_back(peek);
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::EQ, line);
                    }
                    peek = file.get();
                    return Token(value, TokenType::ASSIGN, line);
                    break;
                }
                case '~': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::BITNOT, line);
                }
                case '!': {
                    value.push_back(peek);
                    peek = expectChar(file, '=');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::NEQ, line);
                    }
                    peek = file.get();
                    return Token(value, TokenType::NOT, line);
                }

                // Brackets.
                case '(': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::LPAREN, line);
                }
                case ')': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::RPAREN, line);
                }
                case '[': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::LSBRACKET, line);
                }
                case ']': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::RSBRACKET, line);
                }
                case '{': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::LBRACE, line);
                }
                case '}': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::RBRACE, line);
                }

                case ',': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::COMMA, line);
                }
                case '.': {
                    value.push_back(peek);
                    peek = file.get();
                    // Check if the successor is a digit.
                    if (!isdigit(peek)) {
                        return Token(value, TokenType::PERIOD, line);
                    } else {
                        // If the successor is a digit, recover from the status
                        // before and break.
                        value.pop_back();
                        file.unget();
                        peek = '.';
                        break;
                    }
                }
                case ':': {
                    value.push_back(peek);
                    peek = expectChar(file, ':');
                    if (peek && peek != -1) {
                        value.push_back(peek);
                        peek = file.get();
                        return Token(value, TokenType::DBL_COLON, line);
                    }
                    peek = file.get();
                    return Token(value, TokenType::COLON, line);
                }
                case ';': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::SEMICOLON, line);
                }
                case '?': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::QUESTION_MARK, line);
                }

                case '#': {
                    value.push_back(peek);
                    peek = file.get();
                    return Token(value, TokenType::SHARP, line);
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

            if (auto token_pair = keywordMap.find(value); token_pair != keywordMap.end()) {
                return Token(token_pair->first, token_pair->second, line);
            } else if (auto token_pair = preprocessorMap.find(value);
                       token_pair != preprocessorMap.end()) {
                return Token(token_pair->first, token_pair->second, line);
            }

            return Token(value, TokenType::IDENTIFIER, line);
        }

        // Handle C-style string constant.
        if (peek == '\"') {
            std::string value = "";
            value.push_back(peek);
            while ((peek = file.get()) != '\"') {
                if (file.eof() || peek == '\n') {
                    char errmsg[63];
                    sprintf(errmsg,
                            "At line %d:\n"
                            "String constant doesn't close.\n",
                            line);
                    panic(errmsg);
                    return Token();
                }
                value.push_back(peek);
            }
            value.push_back(peek);
            peek = file.get();
            return Token(value, TokenType::STRING_CONST, line);
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
                char errmsg[63];
                sprintf(errmsg,
                        "At line %d:\n"
                        "Character constant includes non-ascii character!\n",
                        line);
                panic(errmsg);
                return Token();
            }
            value.push_back(peek);
            peek = expectChar(file, '\'');
            if (!peek) {
                char errmsg[127];
                sprintf(errmsg,
                        "At line %d:\n"
                        "Quotation mark doesn't close, or there are more than "
                        "1 character in the char constant.\n",
                        line);
                panic(errmsg);
                return Token();
            }
            value.push_back(peek);
            peek = file.get();
            return Token(value, TokenType::CHAR_CONST, line);
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
                        char errmsg[63];
                        sprintf(errmsg,
                                "At line %d:\n"
                                "Character \'.\' appears twice in a float.\n",
                                line);
                        panic(errmsg);
                    }
                    isFloat = true;
                } else if (peek == 'x' || peek == 'X') {
                    if (value.length() != 1 || value[0] != '0') {
                        char errmsg[63];
                        sprintf(errmsg,
                                "At line %d:\n"
                                "Wrong character appeared in a number\n",
                                line);
                        panic(errmsg);
                    }
                    isHex = true;
                } else if (isalpha(peek)) {
                    if (!isHex && !((peek >= 'a' && peek <= 'f') || (peek >= 'A' && peek <= 'F'))) {
                        char errmsg[63];
                        sprintf(errmsg,
                                "At line %d:\n"
                                "Wrong character appeared in a number\n",
                                line);
                        panic(errmsg);
                    }
                } else {
                    break;
                }
                value.push_back(peek);
            }
            if (isFloat) {
                return Token(value, TokenType::FLOAT_CONST, line);
            } else {
                return Token(value, TokenType::INT_CONST, line);
            }
        }
    }
    return Token();
}

std::vector<pluma::Sym> Lexer::tokenize() {
    std::vector<pluma::Sym> symVec;
    while (!this->inputFile.eof()) {
        pluma::Token token = this->scan(this->inputFile);
        if (token.tokenType != pluma::TokenType::UNKNOWN) {
            symVec.push_back(pluma::Terminal{token});
        }
    }
    symVec.push_back(pluma::Terminal{pluma::Token{"EOF", pluma::TokenType::TK_EOF}});
    return symVec;
}

}  // namespace pluma