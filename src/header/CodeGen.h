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
                output << "    mov rax, 60\n";
                output << "    mov rdi, " << exitNode->code->value << "\n";
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