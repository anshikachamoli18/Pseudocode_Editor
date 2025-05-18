#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ast.h"
#include <unordered_map>
#include <unordered_set>
#include <string>

class SemanticAnalyzer {
public:
    void analyze(ASTNode* root);

private:
    std::unordered_map<std::string, std::string> symbolTable;
    std::unordered_set<std::string> functionTable;
    std::unordered_map<std::string, int> functionParamCount;
    std::unordered_map<std::string, std::unordered_set<std::string>> structFields;
    std::unordered_map<std::string, std::string> functionReturnTypes;

    void analyzeNode(ASTNode* node, const std::string& currentFunction = "");
    void checkFunctionCall(ASTNode* node);
    int countArgs(ASTNode* argListNode);
    std::string evaluateExpressionType(ASTNode* expr);
};

#endif
