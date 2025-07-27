#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>

#include "header/tokenisation.h"
#include "header/Parser.h"
#include "header/CodeGen.h"



int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Improper use..." << std::endl;
        std::cerr << "smoke [input file path]" << std::endl;
    }
    std::cout << argv[1] << std::endl;

    std::string contents; {
        std::stringstream contentsStream;
        std::fstream input(argv[1], std::ios::in);
        contentsStream << input.rdbuf();
        contents = contentsStream.str();
    }

    Tokeniser Tokeninator(contents);

    auto tokens = Tokeninator.Tokenise();

    Parser ASTParser(tokens);
    std::unique_ptr<ProgramNode> ProgramNodes = ASTParser.Parse();

    CodeGen codeGenarator(*ProgramNodes);


    for (const auto& token : tokens) {
        std::cout << "Token: ";
        switch (token.type) {
            case TokenType::exit:
                std::cout << "exit";
                break;
            case TokenType::intLit:
                std::cout << "intLit(" << token.value.value() << ")";
                break;
            case TokenType::semi:
                std::cout << "semi";
                break;
        }
        std::cout << std::endl;
    }
    std::cout << codeGenarator.genASM().str() << std::endl;

    {
        std::fstream file("../out.asm", std::ios::out);
        file << codeGenarator.genASM().str();
    }

    system("wsl nasm -felf64 /mnt/c/Users/olive/CLionProjects/untitled/out.asm");
    system("wsl ld -o /mnt/c/Users/olive/CLionProjects/untitled/out /mnt/c/Users/olive/CLionProjects/untitled/out.o");
    return EXIT_SUCCESS;
}