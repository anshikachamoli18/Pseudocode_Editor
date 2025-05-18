#include "../include/lexer.h"
#include "../include/parser.h"
#include "semantic_analyzer.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include "../include/ir_generator.h"
#include "../include/ir_optimizer.h"
#include "../include/ir_interpreter.h"

using namespace std;
namespace fs = std::filesystem;

void printAST(const unique_ptr<ASTNode>& node, ostream& parsedFile, int depth = 0) {
    for (int i = 0; i < depth; i++) parsedFile << "  ";
    parsedFile << node->type << ": " << node->value << endl;
    for (const auto& child : node->children) {
        printAST(child, parsedFile, depth + 1);
    }
}

int main() {
    // Get current working directory (should be compiler/build/Debug/)
    fs::path cwd = fs::current_path();

    // Construct path to tests directory relative to cwd
    fs::path testsDir = cwd.parent_path().parent_path() / "tests";

    // Define all paths inside tests
    fs::path inputPath = testsDir / "input.txt";
    fs::path tokensPath = testsDir / "tokens.txt";
    fs::path astPath = testsDir / "ast.txt";
    fs::path irPath = testsDir / "ir_generated.txt";
    fs::path optIrPath = testsDir / "optimized_ir.txt";
    fs::path finalOutputPath = testsDir / "output.txt";

    // Read input code from input.txt
    ifstream inputFile(inputPath);
    if (!inputFile) {
        cerr << "Failed to open input file: " << inputPath << endl;
        return 1;
    }
    string code((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());

    // Lexing
    Lexer lexer(code);
    vector<Token> tokens = lexer.tokenize();

    // Write tokens to file
    ofstream tokenFile(tokensPath);
    for (const Token& token : tokens) {
        tokenFile << token.type << " -> " << token.value << endl;
    }

    // Parsing
    Parser parser(tokens);
    unique_ptr<ASTNode> root = parser.parse();

    // Print AST
    ofstream astFile(astPath);
    printAST(root, astFile);

    // Semantic Analysis
    SemanticAnalyzer semanticAnalyzer;
    semanticAnalyzer.analyze(root.get());

    // IR Generation
    IRGenerator irGen(irPath.string());
    irGen.generate(root.get());

    // IR Optimization
    IROptimizer optimizer;
    optimizer.optimize(irPath.string(), optIrPath.string());

    // IR Interpretation - output written to output.txt
    //ofstream execOutput(finalOutputPath);
    IRInterpreter executor;
    executor.interpret(optIrPath.string());

    return 0;
}
