#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>


enum class TokenType {
    _return,
    intLit,
    semi
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

std::vector<Token> Tokenise(std::string fileContents) {
    std::vector<Token> tokens;
    for (size_t i = 0; i < fileContents.size();) {
        if (std::isspace(fileContents[i])) {
            i+= 1;
            continue;
        } else if (std::isalpha(fileContents[i])) {
            if (fileContents.compare(i, 6, "return")==0) {
                tokens.push_back({TokenType::_return});
                i += 6;
            }
        } else if (std::isdigit(fileContents[i])) {
            std::string number;
            while (std::isdigit(fileContents[i])) {
                number += fileContents[i];
                i += 1;
            }
            tokens.push_back({TokenType::intLit, number});
        } else if (fileContents[i] == ';') {
            tokens.push_back({TokenType::semi});
            i+= 1;
        }
    }
    return tokens;
}

std::string TokensToAsm(std::vector<Token>& tokens) {
    std::stringstream output;
    output << "global _start\n";
    output << "_start:\n";
    for (int i = 0; i < tokens.size(); i++) {
        Token& token = tokens.at(i);
        if (token.type == TokenType::_return) {
            if (i+1 < tokens.size() && tokens.at(i + 1).type == TokenType::intLit) {
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
                    output << "    mov rax, 60\n";
                    output << "    mov rdi, " << tokens.at(i+1).value.value() << "\n";
                    output << "    syscall\n";
                }
            }
        }
    }
    return output.str();
}

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

    auto tokens = Tokenise(contents);

    std::cout << contents << std::endl;

    for (const auto& token : tokens) {
        std::cout << "Token: ";
        switch (token.type) {
            case TokenType::_return:
                std::cout << "return";
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
    std::cout << TokensToAsm(tokens) << std::endl;

    {
        std::fstream file("../out.asm", std::ios::out);
        file << TokensToAsm(tokens);
    }

    system("wsl nasm -felf64 /mnt/c/Users/olive/CLionProjects/untitled/out.asm");
    system("wsl ld -o /mnt/c/Users/olive/CLionProjects/untitled/out /mnt/c/Users/olive/CLionProjects/untitled/out.o");
    return EXIT_SUCCESS;
}