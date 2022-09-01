#include <unistd.h>

#include "Ast.hpp"
#include "Formatter.h"
#include "Lexer.h"
#include "Parser.h"
// #include "main.h"

std::fstream inputFile;

void panic(const char *info) {
    std::cerr << "\nError: " << info << std::endl;
    inputFile.close();
    std::abort();
}

int main(int argc, char *argv[]) {
    int opt;
    std::string inputFileName, outputFileName;

    while ((opt = getopt(argc, argv, "o:")) != -1) {
        switch (opt) {
            case 'o':
                outputFileName = optarg;
                std::cout << "-o:" << optarg << std::endl;
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-o output_file]  input_file\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Input file needed\n");
        exit(EXIT_FAILURE);
    }

    inputFileName = argv[optind];

    inputFile.open(inputFileName);
    if (!inputFile.is_open()) {
        panic("Failed to open file!\n");
    }

    pluma::Lexer lexer;
    std::vector<pluma::Sym> symVec;
    while (!inputFile.eof()) {
        pluma::Token token = lexer.scan(inputFile);
        if (token.tokenType != pluma::TokenType::UNKNOWN) {
            symVec.push_back(pluma::Terminal{token});
        }
    }
    inputFile.close();
    symVec.push_back(pluma::Terminal{pluma::Token{"EOF", pluma::TokenType::TK_EOF}});

    pluma::Parser parser;
    parser.grammarPtr->displayAllRule();
    parser.grammarPtr->checkLR1();
    pluma::Ast ast = parser.grammarPtr->gen(symVec);
    ast.display();

    pluma::Formatter formatter(outputFileName);
    formatter.format(ast);

    return 0;
}