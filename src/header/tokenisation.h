#pragma once

#include <iostream>
#include <optional>
#include <vector>


enum class TokenType {
    exit,
    intLit,
    semi
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

class Tokeniser {
    public:
        inline explicit Tokeniser(std::string& fileContents)
            : m_src(std::move(fileContents)), m_CurrentIndex(0) {

        }

        inline std::vector<Token> Tokenise() {
            std::vector<Token> tokens;
            while (auto ch = current()) {
                std::cout << "CURRENT CH WE ARE CHECKING: " << *ch << std::endl;
                if (std::isspace(*ch)) {
                    std::cout << "we are at isspace Check: " << *ch << std::endl;
                    consume();
                    continue;
                }

                if (*ch == ';') {
                    std::cout << "we are at semi: " << std::endl;
                    consume();
                    tokens.push_back({TokenType::semi});
                    continue;
                }

                if (std::isalpha(*ch)) {
                    std::cout << "we are at isalpha Check: " << *ch << std::endl;
                    std::string ident;
                    while (auto c = current()) {
                        if (isspace(*c)) {
                            break;
                        }
                        std::cout << "IDENT SHIT: " << ident << std::endl;
                        if (std::isalpha(*c)) {
                            ident += consume();
                        }
                    }
                    if (ident == "exit") {
                        tokens.push_back({TokenType::exit});
                    } else {
                        std::cerr << "Unknown identifier" << ident << std::endl;
                    }
                    continue;
                }

                if (std::isdigit(*ch)) {
                    std::cout << "we are at isDigit Check: " << *ch << std::endl;
                    std::string number;
                    while (auto c = current()) {
                        std::cout << "we are checking this letter (in the in digit check): " << *c << std::endl;
                        if (std::isdigit(*c)) {
                            number += consume();
                            std::cout << "we are at number: " << number << std::endl;
                        } else {
                            break;
                        }
                    }

                    tokens.push_back({TokenType::intLit, number});
                    continue;
                }

                std::cerr << "Unexpected character: " << *ch << std::endl;
                consume();
            }
            return tokens;
        };

    private:
        const std::string m_src;
        int m_CurrentIndex;

        [[nodiscard]] inline std::optional<char> peek(const size_t offset = 1) const {
            if (m_CurrentIndex + offset >= m_src.length()) {
                return {};
            } else {
                return m_src.at(m_CurrentIndex+offset);
            }
        }

        char consume() {
            return m_src[m_CurrentIndex++];
        }

        [[nodiscard]] inline std::optional<char> current() const {
            if (m_CurrentIndex >= m_src.length()) {
                return {};
            }
            return m_src.at(m_CurrentIndex);
        }
};
