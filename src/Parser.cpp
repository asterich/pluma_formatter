#include "Parser.h"

namespace pluma {

Parser::Parser() : grammarPtr() {}

Parser::Parser(Parser &&other) : grammarPtr(std::move(other.grammarPtr)) {}

Parser &Parser::operator=(Parser &&rhs) {
    if (this == &rhs) {
        return *this;
    }
    this->grammarPtr = std::move(rhs.grammarPtr);
    return *this;
}

}  // namespace pluma