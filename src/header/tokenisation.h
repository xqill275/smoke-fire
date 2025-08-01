#pragma once

#include <iostream>
#include <optional>
#include <vector>


enum class TokenType {
    exit,
    intLit,
    semi,
    _int,
    identifier,
    equals,
    mul,
    plus,
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
                    std::cout << "Pushed semi" << std::endl;
                    continue;
                }

                if (*ch == '*') {
                    consume();
                    tokens.push_back({TokenType::mul});
                    std::cout << "Pushed mul" << std::endl;
                    continue;
                }

                if (*ch == '+') {
                    consume();
                    tokens.push_back({TokenType::plus});
                    std::cout << "Pushed plus" << std::endl;
                    continue;
                }

                if (*ch == '=') {
                    consume();
                    tokens.push_back({TokenType::equals});
                    std::cout << "Pushed equals" << std::endl;
                    continue;
                }

                if (std::isalpha(*ch)) {
                    std::cout << "we are at isalpha Check: " << *ch << std::endl;
                    std::string ident;
                    while (auto c = current()) {
                        if (!std::isalnum(*c) && *c != '_') {
                            // Exit the identifier if the character is not alphanumeric or underscore
                            break;
                        }
                        ident += consume();
                    }
                    if (ident == "exit") {
                        tokens.push_back({TokenType::exit});
                        std::cout << "Pushed exit" << std::endl;
                    } else if (ident == "int") {
                        tokens.push_back({TokenType::_int});
                        std::cout << "Pushed int" << std::endl;
                    } else {
                        tokens.push_back({TokenType::identifier, ident});
                        std::cout << "Pushed identifier" << std::endl;
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
                    std::cout << "Pushed intLiteral" << std::endl;

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
