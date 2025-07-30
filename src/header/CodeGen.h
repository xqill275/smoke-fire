#pragma once

#include <sstream>
#include <unordered_map>


#include "../header/Parser.h"
#include "../header/tokenisation.h"


class CodeGen {
public:
    CodeGen(ProgramNode& program) : m_program(program) {}

    std::stringstream genASM() {
        std::stringstream output;
        output << "global _start\n";
        output << "_start:\n";
        output << "    push rbp\n";
        output << "    mov rbp, rsp\n";
        output << "    sub rsp, 128\n";

        for (const auto& stmt : m_program.statements) {
            if (stmt->type == ASTNodeType::VarDecl) {
                auto* varNode = static_cast<VarDeclNode*>(stmt.get());
                m_stackOffset += 8;
                m_variableOffsets[varNode->name] = m_stackOffset;

                genExpr(output, varNode->value.get());
                output << "    mov QWORD [rbp-" << m_stackOffset << "], rax\n";

            } else if (stmt->type == ASTNodeType::ExitStmt) {
                auto* exitNode = static_cast<ExitStmtNode*>(stmt.get());

                genExpr(output, exitNode->code.get());
                output << "    mov rdi, rax\n";
                output << "    mov rax, 60\n";
                output << "    syscall\n";
            }
        }

        return output;
    }
private:
    ProgramNode& m_program;
    std::unordered_map<std::string, int> m_variableOffsets;
    int m_stackOffset = 0;


    void genExpr(std::stringstream& output, ASTNode* expr) {
        if (expr->type == ASTNodeType::IntLiteral) {
            auto* lit = static_cast<IntLiteralNode*>(expr);
            output << "    mov rax, " << lit->value << "\n";
        } else if (expr->type == ASTNodeType::VarRef) {
            auto* var = static_cast<VarRefNode*>(expr);
            int offset = m_variableOffsets.at(var->name);
            output << "    mov rax, QWORD [rbp-" << offset << "]\n";
        } else if (expr->type == ASTNodeType::BinaryExpr) {
            auto* bin = static_cast<BinaryExprNode*>(expr);
            genExpr(output, bin->right.get());
            output << "    push rax\n";
            genExpr(output, bin->left.get());
            output << "    pop rcx\n";
            switch (bin->op) {
                case BinaryOp::Add:
                    output << "    add rax, rcx\n";
                    break;
                case BinaryOp::Mul:
                    output << "    imul rax, rcx\n";
                    break;
            }
        } else {
            throw std::runtime_error("Unsupported expression in codegen");
        }
    }

};