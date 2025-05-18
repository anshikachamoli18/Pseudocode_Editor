#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include <unordered_map>
#include "token.h"

using namespace std;

class Lexer {
private:
    string input;
    size_t pos;
    char currentChar;
    unordered_map<string, TokenType> keywords;

    void advance();
    void skipWhitespace();
    Token getNumber();
    Token getString();
    Token getIdentifier();
    Token getOperator();
    Token getSeparator();
    

public:
    Lexer(const string& input);
    vector<Token> tokenize();
};

#endif // LEXER_H
