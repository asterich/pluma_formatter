#ifndef PARSER_H_
#define PARSER_H_

#include <memory>

#include "Grammar.hpp"

namespace pluma {

struct Parser {
    std::unique_ptr<Grammar> grammarPtr;

    Parser();
    Parser(const Parser &other) = delete;
    Parser(Parser &&other);
    Parser &operator=(const Parser &rhs) = delete;
    Parser &operator=(Parser &&rhs);

    virtual void genGrammar() = 0;
};

}  // namespace pluma

#endif