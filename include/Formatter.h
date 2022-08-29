#ifndef FORMATTER_H_
#define FORMATTER_H_

#include <fstream>

#include "Ast.hpp"

namespace pluma {

struct Formatter {
   private:
    std::ofstream out;
    static constexpr size_t indentSize = 4;

   private:
    void formatNode(const AstNode *, const size_t indents);
    void printIndents(const size_t indents);

   private:
    Formatter(const Formatter &) = delete;
    Formatter(Formatter &&) = delete;
    Formatter() = delete;

   public:
    Formatter(std::string filename);
    void format(const Ast &);
    ~Formatter();
};

}  // namespace pluma

#endif