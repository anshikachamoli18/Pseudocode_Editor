#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "ast.h"
#include "token.h"

class Parser {
private:
    std::vector<Token> tokens;
    size_t currentPos;

    // Token utilities
    Token currentToken();
    void advance();
    void expect(const std::string& expectedValue);
    Token peek();              // NEW: Look ahead to next token
    Token previousToken();     // NEW: Look back to previous token

    // Helper for IF/ELSE-IF logic
    void parseConditionAndBlock(std::unique_ptr<ASTNode>& ifNode); // NEW

    // Parsers for different constructs
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseLoopStatement();
    std::unique_ptr<ASTNode> parsePrintStatement();
    std::unique_ptr<ASTNode> parseFunctionDeclaration();
    std::unique_ptr<ASTNode> parseStructDeclaration();
    std::unique_ptr<ASTNode> parseArrayAccess();
    std::unique_ptr<ASTNode> parseFunctionCall();
    std::unique_ptr<ASTNode> parseArithmeticExpression();
    std::unique_ptr<ASTNode> parseRelationalExpression();
    std::unique_ptr<ASTNode> parseInputStatement();
    std::vector<std::string> parseParameterList();

public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<ASTNode> parse();
};

#endif // PARSER_H
