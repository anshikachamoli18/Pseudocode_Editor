#include "semantic_analyzer.h"
#include <iostream>

void SemanticAnalyzer::analyze(ASTNode* root) {
    //std::cout << "Starting Semantic Analysis..." << std::endl;
    analyzeNode(root);
    //std::cout << "Semantic Analysis Completed!" << std::endl;
}

void SemanticAnalyzer::analyzeNode(ASTNode* node, const std::string& currentFunction) {
    if (!node) return;

    if (node->type == "KEYWORD" || node->type == "OPERATOR" || node->type == "PUNCTUATION" || node->type == "NUMBER") {
        return;
    }

    // Handle function declaration
    if (node->type == "FunctionDeclaration") {
        std::string funcName = node->value;
        functionTable.insert(funcName);

        if (node->children.size() >= 2) {
            ASTNode* paramList = node->children[1].get();
            int paramCount = static_cast<int>(paramList->children.size());
            functionParamCount[funcName] = paramCount;

            // Declare parameters in symbolTable
            for (const auto& param : paramList->children) {
                symbolTable[param->value] = "int"; // Assume all are int for simplicity
            }
        }

        // Analyze function body
        for (size_t i = 2; i < node->children.size(); ++i) {
            analyzeNode(node->children[i].get(), funcName);
        }

        return;
    }

    // Handle return statement
    if (node->type == "ReturnStatement") {
        std::string returnType = evaluateExpressionType(node->children[0].get());

        if (functionReturnTypes.find(currentFunction) == functionReturnTypes.end()) {
            functionReturnTypes[currentFunction] = returnType;
        } else if (functionReturnTypes[currentFunction] != returnType) {
            std::cout << "Semantic Error: Inconsistent return types in function '" << currentFunction << "'.\n";
        }
    }

    // Handle struct declarations
    if (node->type == "StructDeclaration") {
        std::string structName = node->value;
        std::unordered_set<std::string> fields;
        for (const auto& field : node->children[0]->children) {
            fields.insert(field->value);
        }
        structFields[structName] = fields;
    }

    // Handle assignment expression
    if (node->type == "Expression" && node->value == "=") {
        std::string varName = node->children[0]->value;
        std::string exprType = evaluateExpressionType(node->children[1].get());
        symbolTable[varName] = exprType;
    }

    // Handle function call
    if (node->type == "FunctionCall") {
        checkFunctionCall(node);
    }

    // Handle struct field access validation
    if (node->type == "StructFieldAccess") {
        std::string structVar = node->children[0]->value;
        std::string field = node->children[1]->value;
        std::string structType = symbolTable[structVar];

        if (structFields.find(structType) != structFields.end()) {
            if (structFields[structType].find(field) == structFields[structType].end()) {
                std::cout << "Semantic Error: Field '" << field << "' not found in struct '" << structType << "'.\n";
            }
        } else {
            std::cout << "Semantic Error: Struct type '" << structType << "' not declared.\n";
        }
    }

    if (node->type == "InputStatement") {
        std::string identifier = node->value;
    
        // Implicitly declare the variable with unknown type (or any default)
        if (symbolTable.find(identifier) == symbolTable.end()) {
            symbolTable[identifier] = "unknown";  // or "any", "int", etc. if defaulting
        }
    
        // Optional: You can prompt for type inference here later if needed
    }
    
    // Recursively analyze child nodes
    for (auto& child : node->children) {
        analyzeNode(child.get(), currentFunction);
    }
}


void SemanticAnalyzer::checkFunctionCall(ASTNode* node) {
    if (node->children.empty()) return;

    std::string funcName = node->value;

    if (functionParamCount.find(funcName) != functionParamCount.end()) {
        int expected = functionParamCount[funcName];
        int given = countArgs(node->children[0].get());

        if (expected != given) {
            std::cout << "Semantic Error: Function '" << funcName << "' expects " << expected
                      << " parameter(s), but " << given << " were provided.\n";
        }
    } else {
        std::cout << "Semantic Warning: Function '" << funcName << "' called but not declared.\n";
    }
}

int SemanticAnalyzer::countArgs(ASTNode* argListNode) {
    if (!argListNode) return 0;
    return static_cast<int>(argListNode->children.size());
}

std::string SemanticAnalyzer::evaluateExpressionType(ASTNode* expr) {
    if (!expr) return "unknown";

    if (expr->type == "NUMBER") return "int";

    if (expr->type == "STRING") return "string";

    if (expr->type == "Variable") {
        return symbolTable.count(expr->value) ? symbolTable[expr->value] : "unknown";
    }

    if (expr->type == "Expression" && expr->children.size() == 2) {
        std::string left = evaluateExpressionType(expr->children[0].get());
        std::string right = evaluateExpressionType(expr->children[1].get());

        if (left == right) return left;

        return "mismatch";  // Indicates incompatible types
    }

    return "unknown";
}
