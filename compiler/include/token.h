#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

using namespace std;

enum class TokenType {  
    KEYWORD, IDENTIFIER, INTEGER_LITERAL, FLOAT_LITERAL, BOOLEAN_LITERAL,
    STRING, OPERATOR, PUNCTUATION, SEPARATOR, ASSIGNMENT, END_OF_FILE, INVALID
};

// Convert TokenType to string
inline string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INTEGER_LITERAL: return "INTEGER_LITERAL";
        case TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
        case TokenType::BOOLEAN_LITERAL: return "BOOLEAN_LITERAL";
        case TokenType::STRING: return "STRING";
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::PUNCTUATION: return "PUNCTUATION";
        case TokenType::SEPARATOR: return "SEPARATOR"; // NEW ADDITION
        case TokenType::ASSIGNMENT: return "ASSIGNMENT";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}

// Overload operator<< to print TokenType
inline ostream& operator<<(ostream& os, TokenType type) {
    os << tokenTypeToString(type);
    return os;
}

struct Token {
    TokenType type;
    string value;
};

#endif // TOKEN_H
