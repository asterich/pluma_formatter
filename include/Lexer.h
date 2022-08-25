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
    char peek = ' ';
    int line = 1;

   public:
    Token scan(std::fstream &file);
};

}  // namespace pluma

#endif