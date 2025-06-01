#include "lexer.hpp"
#include <cctype>
#include <unordered_set>
#include <iostream>

static const std::unordered_set<std::string> keywords = {
    "if", "else", "elseif", "while", "do", "for", "foreach", "break", "continue",
    "switch", "case", "default", "function", "return", "echo", "print",
    "global", "static", "var", "const", "public", "private", "protected", "final",
    "class", "interface", "trait", "extends", "implements", "abstract",
    "new", "clone", "instanceof", "use", "namespace", "require", "require_once",
    "include", "include_once", "try", "catch", "finally", "throw",
    "match", "readonly", "enum", "fn", "yield", "yield from",
    "true", "false", "null", "and", "or", "xor", "isset", "empty", "unset",
    "__halt_compiler", "goto", "declare", "enddeclare", "endif", "endwhile",
    "endforeach", "endfor", "endswitch", "list"
};

Lexer::Lexer(const std::string& input)
    : input(input), pos(0), line(1), column(1), inPHP(false) {
}
void Lexer::tokenize() {
    // Define the lexer’s internal finite state machine states.
    enum class State {
        Start, Identifier, Number, FloatFraction, String, VariableStart, VariableName,
        Operator, Separator, LineCommentStart, LineComment, BlockCommentStart,
        BlockComment, Whitespace, Accept
    };

    while (pos < input.size()) {
        // Skip insignificant whitespace (e.g., between tokens, outside strings/comments)
        skipWhitespace();

        State state = State::Start;
        std::string lexeme;
        int startLine = line, startCol = column;

        // Check for the start of a PHP block: <?php
        if (!inPHP && match("<?php")) {
            inPHP = true;
            addToken(TokenType::PHPTagOpen, "<?php", startLine, startCol);
            continue;
        }

        // Check for the end of a PHP block: ?>
        if (inPHP && match("?>")) {
            inPHP = false;
            addToken(TokenType::PHPTagClose, "?>", startLine, startCol);
            continue;
        }

        // If not in PHP, treat the input as raw HTML text
        if (!inPHP) {
            std::string html;
            while (pos < input.size() && !matchAhead("<?php")) {
                html += get();  // Collect HTML until the start of a PHP block
            }
            if (!html.empty()) {
                addToken(TokenType::HTMLText, html, startLine, startCol);
            }
            continue;
        }

        // Process PHP code inside <?php ... ?>
        while (pos < input.size() && state != State::Accept) {
            char c = peek();

            switch (state) {
            case State::Start:
                // Decide the token type based on the current character
                if (std::isspace(c)) state = State::Whitespace;
                else if (std::isalpha(c) || c == '_') state = State::Identifier;
                else if (std::isdigit(c)) state = State::Number;
                else if (c == '\'' || c == '"') state = State::String;
                else if (c == '$') state = State::VariableStart;
                else if (c == '/' && matchAhead("//")) state = State::LineCommentStart;
                else if (c == '/' && matchAhead("/*")) state = State::BlockCommentStart;
                else if (std::string("(){}[];,.").find(c) != std::string::npos) state = State::Separator;
                else state = State::Operator;
                break;

            case State::Whitespace:
                // Consume and emit whitespace as a token
                while (std::isspace(peek())) lexeme += get();
                addToken(TokenType::Whitespace, lexeme, startLine, startCol);
                state = State::Accept;
                break;

            case State::Identifier:
                // Parse keywords and identifiers
                while (std::isalnum(peek()) || peek() == '_') lexeme += get();
                addToken(keywords.count(lexeme) ? TokenType::Keyword : TokenType::Identifier, lexeme, startLine, startCol);
                state = State::Accept;
                break;

            case State::Number:
                // Parse integer and float literals
                while (std::isdigit(peek())) lexeme += get();
                if (peek() == '.') {
                    lexeme += get();  // Start of float
                    state = State::FloatFraction;
                }
                else {
                    addToken(TokenType::IntegerLiteral, lexeme, startLine, startCol);
                    state = State::Accept;
                }
                break;

            case State::FloatFraction:
                // Parse the fractional part of a float
                while (std::isdigit(peek())) lexeme += get();
                addToken(TokenType::FloatLiteral, lexeme, startLine, startCol);
                state = State::Accept;
                break;

            case State::String: {
                // Parse quoted string literal (single or double quotes)
                char quote = get();  // Consume opening quote
                while (peek() != quote && pos < input.size()) lexeme += get();
                if (peek() == quote) {
                    get();  // Consume closing quote
                    addToken(TokenType::StringLiteral, lexeme, startLine, startCol);
                }
                else {
                    error("Unterminated string", startLine, startCol);
                }
                state = State::Accept;
                break;
            }

            case State::VariableStart:
                // Parse variable: starts with $
                get();  // Consume '$'
                lexeme = "$";
                if (std::isalpha(peek()) || peek() == '_') {
                    state = State::VariableName;
                }
                else {
                    // Standalone '$' treated as operator
                    addToken(TokenType::Operator, "$", startLine, startCol);
                    state = State::Accept;
                }
                break;

            case State::VariableName:
                // Parse variable name after $
                while (std::isalnum(peek()) || peek() == '_') lexeme += get();
                addToken(TokenType::Variable, lexeme, startLine, startCol);
                state = State::Accept;
                break;

            case State::LineCommentStart:
                // Begin parsing single-line comment
                match("//");  // Consume both slashes
                state = State::LineComment;
                break;

            case State::LineComment:
                // Consume until end of line or file
                while (peek() != '\n' && pos < input.size()) lexeme += get();
                addToken(TokenType::Comment, lexeme, startLine, startCol);
                state = State::Accept;
                break;

            case State::BlockCommentStart:
                // Begin parsing multi-line comment
                match("/*");  // Consume opening
                state = State::BlockComment;
                break;

            case State::BlockComment:
                // Consume until closing */
                while (!matchAhead("*/") && pos < input.size()) lexeme += get();
                if (match("*/")) {
                    addToken(TokenType::Comment, lexeme, startLine, startCol);
                }
                else {
                    error("Unterminated block comment", startLine, startCol);
                }
                state = State::Accept;
                break;

            case State::Separator:
                // Single-character separators: (), ;, etc.
                lexeme += get();
                addToken(TokenType::Separator, lexeme, startLine, startCol);
                state = State::Accept;
                break;

            case State::Operator:
                // Fallback: treat anything else as operator (can be extended later)
                lexeme += get();
                addToken(TokenType::Operator, lexeme, startLine, startCol);
                state = State::Accept;
                break;
            }
        }
    }
}



void Lexer::skipWhitespace() {
    while (std::isspace(peek())) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        else {
            column++;
        }
        pos++;
    }
}

char Lexer::peek() const {
    return pos < input.size() ? input[pos] : '\0';
}

char Lexer::get() {
    char c = peek();
    pos++;
    column++;
    return c;
}

bool Lexer::match(const std::string& str) {
    if (input.substr(pos, str.size()) == str) {
        pos += str.size();
        column += str.size();
        return true;
    }
    return false;
}

bool Lexer::matchAhead(const std::string& str) const {
    return input.substr(pos, str.size()) == str;
}

void Lexer::addToken(TokenType type, const std::string& value, int line, int col) {
    table.add({ type, value, line, col });
}

void Lexer::error(const std::string& msg, int line, int col) {
    std::cerr << "Lexical Error (" << line << ":" << col << "): " << msg << "\n";
}

const TokenTable& Lexer::getTokens() const {
    return table;
}
