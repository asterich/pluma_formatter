#include "main.h"

std::fstream file;
std::fstream outfile;

void panic(const char *info) {
    std::cerr << "Error: " << info << std::endl;
    file.close();
    outfile.close();
    std::abort();
}

int main() {
    file.open("../../test.txt");
    if (!file.is_open()) {
        panic("Failed to open file!\n");
    }

    Lexer lexer;
    while (!file.eof()) {
        Token token = lexer.scan(file);
        if (token.tokenType != TokenType::UNKNOWN)
            std::cout << "Token: { " << token.value << " " << token.tokenType
                      << " }\n";
    }

    return 0;
}