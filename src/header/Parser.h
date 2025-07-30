#pragma once
#include <vector>
#include <bits/unique_ptr.h>

#include "../header/tokenisation.h"



enum class BinaryOp {
    Add,
    Mul
};

enum class ASTNodeType {
    ExitStmt,
    IntLiteral,
    Program,
    VarDecl,
    VarRef,
    BinaryExpr
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
    std::unique_ptr<ASTNode> value;

    VarDeclNode(const std::string& n, std::unique_ptr<ASTNode> v) : name(n), value(std::move(v)) {
        type = ASTNodeType::VarDecl;
    }
};

struct VarRefNode : public ASTNode {
    std::string name;
    VarRefNode(const std::string& n) : name(n) {
        type = ASTNodeType::VarRef;
    }
};

struct BinaryExprNode: public ASTNode {
    BinaryOp op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryExprNode(BinaryOp o, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r) : op(o), left(std::move(l)), right(std::move(r)) {
        type = ASTNodeType::BinaryExpr;
    }

};


class Parser {
public:
    explicit Parser(std::vector<Token>& Tokens) : m_tokens(Tokens) {}

    std::unique_ptr<ProgramNode> Parse(bool DEBUG) {
        std::cout << "we are now parsing" << std::endl;
        auto program = std::make_unique<ProgramNode>();
        std::cout << "make program node" << std::endl;
        while (!isAtEnd()) {
            std::cout << "in the !isAtEnd Check" << std::endl;
            auto stmt = parseStatement();
            if (stmt) {
                std::cout << "running stmt parse" << std::endl;
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

    void printExpr(const ASTNode* node) {
        switch (node->type) {
            case ASTNodeType::IntLiteral: {
                const auto* lit = static_cast<const IntLiteralNode*>(node);
                std::cout<< lit->value;
                break;
            }
            case ASTNodeType::VarRef: {
                const auto* var = static_cast<const VarRefNode*>(node);
                std::cout << var->name;
                break;
            }
            case ASTNodeType::BinaryExpr: {
                const auto* bin = static_cast<const BinaryExprNode*>(node);
                std::cout << "(";
                printExpr(bin->left.get());
                switch (bin->op) {
                    case BinaryOp::Add: std::cout << "+"; break;
                    case BinaryOp::Mul: std::cout << "*"; break;
                }
                printExpr(bin->right.get());
                std::cout << ")";
                break;
            }
                default: {
                std::cout << "(unknown expr)";
                break;
            }
        }
    }

    void getDebugProgram(const ProgramNode& program) {
        std::cout << "--- DEBUG: AST DUMP ---" << std::endl;
        for (const auto& stmt : program.statements) {
            switch (stmt->type) {
                case ASTNodeType::ExitStmt: {
                    const auto* exitNode = static_cast<const ExitStmtNode*>(stmt.get());
                    std::cout << "ExitStmt code =";
                    printExpr(exitNode->code.get()); // NEED TO WRITE
                    std::cout << std::endl;
                    break;
                }
                case ASTNodeType::VarDecl: {
                    const auto* varNode = static_cast<const VarDeclNode*>(stmt.get());
                    std::cout << "VarDecl: name = " << varNode->name << ", value = ";
                    printExpr(varNode->value.get()); // NEED TO WRITE
                    std::cout << std::endl;
                    break;
                }
                case ASTNodeType::IntLiteral: {
                    const auto* lit = static_cast<const IntLiteralNode*>(stmt.get());
                    std::cout << "IntLiteral (standalone?): value = " << lit->value << std::endl;
                    break;
                }
                case ASTNodeType::Program: {
                    std::cout << "Program node (should not appear directly in statements)" << std::endl;
                    break;
                }
                case ASTNodeType::BinaryExpr: {
                    std::cout << "BinaryExpr (should not appear directly in statements)" << std::endl;
                    break;
                }
            }
        }
        std::cout << "--- END AST DUMP ---" << std::endl;
    }



private:
    std::vector<Token>& m_tokens;
    size_t m_CurrentIndex{};

    std::unique_ptr<ASTNode> parseStatement() {
        std::cout << "in parseStatement function" << std::endl;
        const Token& tok = current();
        if (tok.type == TokenType::exit) {
            advance();
            auto expr = parseExpression();
            if (current().type != TokenType::semi) error("Expected semicolon");
            advance();
            return std::make_unique<ExitStmtNode>(std::move(expr));
        }

        if (tok.type == TokenType::_int) {
            std::cout << "found int statement" << std::endl;
            advance();
            const auto& identTok = current();
            std::cout << "found the ident" << std::endl;
            if (identTok.type != TokenType::identifier) error("Expected identifier");
            std::string name = identTok.value.value();
            std::cout << "found name " << name << std::endl;
            advance();

            if (current().type != TokenType::equals) error("Expected =");
            advance();

            auto expr = parseExpression();

            if (current().type != TokenType::semi) error("Expected semicolon");
            advance();
            std::cout << "returning unique varDeclNode" << std::endl;
            return std::make_unique<VarDeclNode>(name, std::move(expr));
        }
    }

    std::unique_ptr<ASTNode> parseTerm() {
        std::cout << "we are now in parseTerm" << std::endl;
        auto left = parseFactor();
        while (!isAtEnd() && current().type == TokenType::mul) {
            advance();
            auto right = parseFactor();
            left = std::make_unique<BinaryExprNode>(BinaryOp::Mul, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ASTNode> parseExpression() {
        std::cout << "we are now in parseExpression function" << std::endl;
        auto left = parseTerm(); // NEED TO MAKE
        while (!isAtEnd() && current().type == TokenType::plus) {
            advance();
            auto right = parseTerm();
            left = std::make_unique<BinaryExprNode>(BinaryOp::Add, std::move(left), std::move(right));
        }
        return left;
    }


    std::unique_ptr<ASTNode> parseFactor() {
        std::cout << "we are now in parseFactor" << std::endl;
        const auto& tok = current();
        std::cout << "(parseFactor)gotCurrent" << std::endl;
        if (tok.type == TokenType::intLit) {
            std::cout << "(parseFactor)Current is a intLit" << std::endl;
            int value = std::stoi(tok.value.value());
            std::cout << "(parseFactor)creating value " << value << std::endl;
            advance();
            std::cout << "(parseFactor)made IntLitNode" << std::endl;
            return std::make_unique<IntLiteralNode>(value);
        } else if (tok.type == TokenType::identifier) {
            std::cout << "(parseFactor)Current is a identifier" << std::endl;
            std::string name = tok.value.value();
            advance();
            return std::make_unique<VarRefNode>(name);
        } else {
            error("EXpected number or identifier");
            return nullptr;
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
