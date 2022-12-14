#include <unistd.h>

#include "Ast.hpp"
#include "Formatter.h"
#include "Lexer.h"
#include "c/CParser.h"
// #include "main.h"

void panic(const char *info) {
    std::cerr << "\nError: " << info << std::endl;
    std::abort();
}

int main(int argc, char *argv[]) {
    int opt;
    std::string inputFilename, outputFilename;

    while ((opt = getopt(argc, argv, "o:")) != -1) {
        switch (opt) {
            case 'o':
                outputFilename = optarg;
                // std::cout << "-o:" << optarg << std::endl;
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

    inputFilename = argv[optind];

    pluma::Lexer lexer(inputFilename);
    std::vector<pluma::Sym> symVec = lexer.tokenize();

    std::unique_ptr<pluma::Parser> cParserPtr = std::make_unique<pluma::CParser>(pluma::CParser());
    cParserPtr->grammarPtr->displayAllRule();
    cParserPtr->grammarPtr->checkLR1();
    pluma::Ast ast = cParserPtr->grammarPtr->gen(symVec);
    ast.display();

    pluma::Formatter formatter(outputFilename);
    formatter.format(ast);

    return 0;
}