#ifndef LEXER_H_
#define LEXER_H_

#include <cctype>
#include <fstream>
#include <iostream>
#include <string>

#include "Token.h"
#include "main.h"

class Lexer {
   private:
    char peek = ' ';

   public:
    Token scan(std::fstream &file);
};

#endif