#include "Ast.hpp"
#include "Formatter.h"
#include "Lexer.h"
#include "Parser.h"
// #include "main.h"

std::fstream file;
std::fstream outfile;

void panic(const char *info) {
    std::cerr << "\nError: " << info << std::endl;
    file.close();
    outfile.close();
    std::abort();
}

int main() {
    file.open(
        "/Users/asterich/Desktop/MyPrograms/C++/compiler_learning/pluma_formatter/example/"
        "test.txt");
    if (!file.is_open()) {
        panic("Failed to open file!\n");
    }

    pluma::Lexer lexer;
    std::vector<pluma::Sym> symVec;
    while (!file.eof()) {
        pluma::Token token = lexer.scan(file);
        if (token.tokenType != pluma::TokenType::UNKNOWN) {
            symVec.push_back(pluma::Terminal{token});
        }
    }
    file.close();
    symVec.push_back(pluma::Terminal{pluma::Token{"EOF", pluma::TokenType::TK_EOF}});

    pluma::Parser parser;
    parser.grammarPtr->displayAllRule();
    parser.grammarPtr->checkLR1();
    pluma::Ast ast = parser.grammarPtr->gen(symVec);
    ast.display();

    pluma::Formatter formatter(
        "/Users/asterich/Desktop/MyPrograms/C++/compiler_learning/pluma_formatter/example/out.txt");
    formatter.format(ast);

    return 0;
}