#pragma once
#include <vector>

#include "../header/tokenisation.h"


enum class ASTNodeType {
    ExitStmt,
    IntLiteral,

};

struct ASTNode {
    ASTNodeType type;
};

class Parser {
public:
    explicit Parser(std::vector<Token>& Tokens) : m_tokens(Tokens) {}


private:
    std::vector<Token>& m_tokens;
    size_t m_CurrentIndex{};

    [[nodiscard]] bool isAtEnd() const {
        if (m_CurrentIndex >= m_tokens.size() {
            return true;
        } else {
            return false;
        }
    }

    const Token& current() const {
        return m_tokens.at(m_CurrentIndex);
    }

    void advance() {
        if (!isAtEnd()) {
            m_CurrentIndex++;
        }
    }


};
