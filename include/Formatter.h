#ifndef FORMATTER_H_
#define FORMATTER_H_

#include "Ast.hpp"

namespace pluma {

static constexpr size_t indentSize = 4;

void formatNode(const AstNode *, const size_t indents);

void format(const Ast &);

}

#endif