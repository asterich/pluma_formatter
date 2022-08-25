#ifndef PARSER_H_
#define PARSER_H_

#include <memory>

#include "Grammar.hpp"

namespace pluma {

struct Parser {
    std::unique_ptr<Grammar> grammarPtr;

    Parser();
};

}  // namespace pluma

#endif