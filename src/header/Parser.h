#pragma once
#include <vector>
#include <bits/unique_ptr.h>

#include "../header/tokenisation.h"


enum class ASTNodeType {
    ExitStmt,
    IntLiteral,
    Program,
    VarDecl,
    VarRef
};

struct ASTNode {
    ASTNodeType type;
};

struct IntLiteralNode : public ASTNode {
    int value;
    IntLiteralNode(int val) : value(val) { type = ASTNodeType::IntLiteral;}
};

struct ExitStmtNode : public ASTNode {
    std::unique_ptr<ASTNode> code;
    ExitStmtNode(std::unique_ptr<ASTNode> c) : code(std::move(c)) {
        type = ASTNodeType::ExitStmt;
    }
};

struct ProgramNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    ProgramNode() { type = ASTNodeType::Program; }
};

struct VarDeclNode : public ASTNode {
    std::string name;
    std::unique_ptr<IntLiteralNode> value;

    VarDeclNode(const std::string& n, std::unique_ptr<IntLiteralNode> v) : name(n), value(std::move(v)) {
        type = ASTNodeType::VarDecl;
    }
};

struct VarRefNode : public ASTNode {
    std::string name;
    VarRefNode(const std::string& n) : name(n) {
        type = ASTNodeType::VarRef;
    }
};



class Parser {
public:
    explicit Parser(std::vector<Token>& Tokens) : m_tokens(Tokens) {}

    std::unique_ptr<ProgramNode> Parse(bool DEBUG) {
        std::cout << "we are now parsing" << std::endl;
        auto program = std::make_unique<ProgramNode>();

        while (!isAtEnd()) {
            auto stmt = parseStatement();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            } else {
                error("unknown statement");
            }
        }
        if (DEBUG) {
            std::cout << "we are now showing the debug program from the parser" << std::endl;
            getDebugProgram(*program);
        }
        return program;
    }

    void getDebugProgram(const ProgramNode& program) {
        std::cout << "--- DEBUG: AST DUMP ---" << std::endl;
        for (const auto& stmt : program.statements) {
            switch (stmt->type) {
                case ASTNodeType::ExitStmt: {
                    const auto* exitNode = static_cast<const ExitStmtNode*>(stmt.get());
                    std::cout << "ExitStmt: code = ";
                    if (exitNode->code->type == ASTNodeType::IntLiteral) {
                        auto* lit = static_cast<IntLiteralNode*>(exitNode->code.get());
                        std::cout << lit->value;
                    } else if (exitNode->code->type == ASTNodeType::VarRef) {
                        auto* var = static_cast<VarRefNode*>(exitNode->code.get());
                        std::cout << var->name;
                    } else {
                        std::cout << "(unknown expression)";
                    }
                    std::cout << std::endl;
                    break;
                }
                case ASTNodeType::VarDecl: {
                    const auto* varNode = static_cast<const VarDeclNode*>(stmt.get());
                    std::cout << "VarDecl: name = " << varNode->name
                              << ", value = " << varNode->value->value << std::endl;
                    break;
                }
                case ASTNodeType::IntLiteral: {
                    const auto* lit = static_cast<const IntLiteralNode*>(stmt.get());
                    std::cout << "IntLiteral (standalone?): value = " << lit->value << std::endl;
                    break;
                }
                case ASTNodeType::Program:
                    std::cout << "Program node (should not appear directly in statements)" << std::endl;
                    break;
            }
        }
        std::cout << "--- END AST DUMP ---" << std::endl;
    }



private:
    std::vector<Token>& m_tokens;
    size_t m_CurrentIndex{};

    std::unique_ptr<ASTNode> parseStatement() {
        const Token& tok = current();
        if (tok.type == TokenType::exit) {
            advance();
            const Token& intTok = current();

            std::unique_ptr<ASTNode> valueNode;
            if (intTok.type == TokenType::intLit) {
                int value = std::stoi(intTok.value.value());
                valueNode = std::make_unique<IntLiteralNode>(value);
            } else if (intTok.type == TokenType::identifier) {
                valueNode = std::make_unique<VarRefNode>(intTok.value.value());
            } else {
                error("Expected int literal or variable name after exit");
            }
            advance();
            const Token& semiTok = current();
            if (semiTok.type != TokenType::semi) error("Expected semicolon");
            advance();
            return std::make_unique<ExitStmtNode>(std::move(valueNode));
        }

        if (tok.type == TokenType::_int) {
            advance();
            const Token& identTok = current();
            if (identTok.type != TokenType::identifier) error("Expected identifier after int");
            std::string name = identTok.value.value();
            advance();

            const Token& eqTok = current();
            if (eqTok.type != TokenType::equals) error("Expected = after identifier: "+name);
            advance();

            const Token& valueTok = current();
            if (valueTok.type != TokenType::intLit) error("Expected intLit after =");
            advance();

            int value = std::stoi(valueTok.value.value());

            const Token& semiTok = current();
            if (semiTok.type != TokenType::semi) error("Expected ;");
            advance();

            return std::make_unique<VarDeclNode>(name, std::make_unique<IntLiteralNode>(value));
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
