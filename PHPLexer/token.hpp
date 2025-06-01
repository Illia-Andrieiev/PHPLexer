#pragma once
#include <string>

// Enum representing the different types of tokens we can encounter
enum class TokenType {
    Identifier,
    Keyword,
    IntegerLiteral,
    FloatLiteral,
    HTMLText,
    StringLiteral,
    Operator,
    Separator,
    Comment,
    Whitespace,
    PHPTagOpen,
    PHPTagClose,
    Variable,
    Unknown,
};

// Token structure: contains type, value, and source location
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    std::string typeToString() const{
        switch (type) {
        case TokenType::Identifier:     return "Identifier";
        case TokenType::Keyword:        return "Keyword";
        case TokenType::IntegerLiteral: return "IntegerLiteral";
        case TokenType::FloatLiteral:   return "FloatLiteral";
        case TokenType::HTMLText:       return "HTMLText";
        case TokenType::StringLiteral:  return "StringLiteral";
        case TokenType::Operator:       return "Operator";
        case TokenType::Separator:      return "Separator";
        case TokenType::Comment:        return "Comment";
        case TokenType::Whitespace:     return "Whitespace";
        case TokenType::PHPTagOpen:     return "PHPTagOpen";
        case TokenType::PHPTagClose:    return "PHPTagClose";
        case TokenType::Unknown:        return "Unknown";
        case TokenType::Variable:        return "Variable";
        default:                        return "Invalid";
        }
    }
};