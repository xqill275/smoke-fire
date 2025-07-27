#pragma once
#include <vector>
#include <bits/unique_ptr.h>

#include "../header/tokenisation.h"


enum class ASTNodeType {
    ExitStmt,
    IntLiteral,
    Program
};

struct ASTNode {
    ASTNodeType type;
};

struct IntLiteralNode : public ASTNode {
    int value;
    IntLiteralNode(int val) : value(val) { type = ASTNodeType::IntLiteral;}
};

struct ExitStmtNode : public ASTNode {
    std::unique_ptr<IntLiteralNode> code;
    ExitStmtNode(std::unique_ptr<IntLiteralNode> c) : code(std::move(c)) {
        type = ASTNodeType::ExitStmt;
    }
};

struct ProgramNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    ProgramNode() { type = ASTNodeType::Program; }
};



class Parser {
public:
    explicit Parser(std::vector<Token>& Tokens) : m_tokens(Tokens) {}

    std::unique_ptr<ProgramNode> Parse() {
        auto program = std::make_unique<ProgramNode>();

        while (!isAtEnd()) {
            auto stmt = parseStatement();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            } else {
                error("unknown statement");
            }
        }
        return program;
    }



private:
    std::vector<Token>& m_tokens;
    size_t m_CurrentIndex{};

    std::unique_ptr<ASTNode> parseStatement() {
        const Token& tok = current();
        if (tok.type == TokenType::exit) {
            advance();
            const Token& intTok = current();
            if (intTok.type != TokenType::intLit) error("Expected int after exit");
            int value = std::stoi(intTok.value.value());
            advance();

            const Token& semiTok = current();
            if (semiTok.type != TokenType::semi) error("Expected semicolon");
            advance();
            return std::make_unique<ExitStmtNode>(std::make_unique<IntLiteralNode>(value));
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

    bool isAtEnd() const {
        if (m_CurrentIndex >= m_tokens.size()) {
            return true;
        } else {
            return false;
        }
    }

    void error(const std::string& msg) {
        throw std::runtime_error("Parse error: "+msg);
    }


};
