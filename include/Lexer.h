#ifndef LEXER_H_
#define LEXER_H_

#include <cctype>
#include <fstream>
#include <iostream>
#include <string>

#include "Symbol.hpp"
#include "main.h"

namespace pluma {

class Lexer {
   private:
    std::fstream inputFile;
    char peek = ' ';
    int line = 1;

   private:
    Lexer() = delete;
    Lexer(const Lexer &) = delete;
    Lexer(Lexer &&) = delete;
    Token scan(std::fstream &file);

   public:
    Lexer(std::string inputFilename);
    ~Lexer();
    std::vector<pluma::Sym> tokenize();
};

}  // namespace pluma

#endif