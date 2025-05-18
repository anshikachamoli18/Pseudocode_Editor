#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "ast.h"
#include <fstream>
#include <string>
#include <vector>

class IRGenerator {
public:
    IRGenerator(const std::string& outputPath);
    void generate(ASTNode* root);  // Method to generate TAC from AST

private:
    std::ofstream outFile;         // Output file for storing the generated TAC
    int tempVarCount = 0;          // Counter for temporary variable generation

    std::string generateExpression(ASTNode* node); // Generates TAC for an expression
    void generateStatement(ASTNode* node);         // Generates TAC for a statement
    void handleIfElseIf(ASTNode* node);            // Handles if-else-if statements
    void generateIfStatement(ASTNode* node);      // Generates TAC for if statements
    void generateLoopStatement(ASTNode* node);    // Generates TAC for loop statements
    void generateFunctionCall(ASTNode* node);     // Generates TAC for function calls
    std::string newTemp();                        // Generates a new temporary variable for TAC
};

#endif
