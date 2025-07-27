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
                output << "    mov QWORD [rbp-" << m_stackOffset << "], " << varNode->value->value << "\n";
            } else if (stmt->type == ASTNodeType::ExitStmt) {
                auto* exitNode = static_cast<ExitStmtNode*>(stmt.get());
                if (exitNode->code->type == ASTNodeType::IntLiteral) {
                    auto* lit = static_cast<IntLiteralNode*>(exitNode->code.get());
                    output << "    mov rdi, " << lit->value << "\n";
                } else if (exitNode->code->type == ASTNodeType::VarRef) {
                    auto* var = static_cast<VarRefNode*>(exitNode->code.get());
                    int offset = m_variableOffsets.at(var->name);
                    output << "    mov rdi, QWORD [rbp-" << offset << "]\n";
                }

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

};