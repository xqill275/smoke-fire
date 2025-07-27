#pragma once

#include <sstream>


#include "../header/Parser.h"
#include "../header/tokenisation.h"


class CodeGen {
public:
    CodeGen(ProgramNode& program) : m_program(program) {}

    std::stringstream genASM() {
        std::stringstream output;
        output << "global _start\n";
        output << "_start:\n";

        for (const auto& stmt : m_program.statements) {
            if (stmt->type == ASTNodeType::ExitStmt) {
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
};