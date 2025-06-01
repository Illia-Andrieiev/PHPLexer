#pragma once
#include "token_table.hpp"
#include <string>

// Lexer class using a finite automaton (FA) approach
class Lexer {
public:
    Lexer(const std::string& input);
    void tokenize();
    const TokenTable& getTokens() const;

private:

    std::string input;
    size_t pos;
    int line, column;
    TokenTable table;

    bool inPHP;

    void skipWhitespace();
    char peek() const;
    char get();
    bool match(const std::string& str);
    bool matchAhead(const std::string& str) const;
    void addToken(TokenType type, const std::string& value, int line, int col);
    void error(const std::string& msg, int line, int col);
};