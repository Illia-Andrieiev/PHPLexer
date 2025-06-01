#pragma once
#include "token.hpp"
#include <vector>
// A class that stores and prints the list of tokens found by the lexer
class TokenTable {
    std::vector<Token> tokens;
public:
    void add(const Token& token);
    void print() const;
    void printIndent(int indent) const;

};
