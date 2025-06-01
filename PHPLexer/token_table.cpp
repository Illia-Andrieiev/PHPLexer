#include "token.hpp"
#include "token_table.hpp"
#include <vector>
#include <iostream>

// Add token
void TokenTable::add(const Token& token) {
    tokens.push_back(token);
}

// Print token table
void TokenTable::print() const {
    for (const auto& tok : tokens) {
        std::cout << "Token(" << tok.typeToString()
            << ", \"" << tok.value << "\", Line: "
            << tok.line << ", Col: " << tok.column << ")\n";
    }

    std::cout << "\n--- Hierarchical Structure ---\n";
    int indent = 0;

    for (const auto& tok : tokens) {
        // Increase indent after opening blocks
        if (tok.value == "{" || tok.value == "(") {
            printIndent(indent);
            std::cout << tok.value << "\n";
            indent++;
        }
        // Decrease indent after closing blocks
        else if (tok.value == "}" || tok.value == ")") {
            indent--;
            printIndent(indent);
            std::cout << tok.value << "\n";
        }
        // Handle semicolon-level items
        else if (tok.value == ";") {
            printIndent(indent);
            std::cout << tok.value << "\n";
        }
        // Print everything else
        else {
            printIndent(indent);
            std::cout << tok.value << "\n";
        }
    }

    std::cout << "-----------------------------\n";
}

void TokenTable::printIndent(int indent) const{
    for (int i = 0; i < indent; ++i) std::cout << "  ";
}

