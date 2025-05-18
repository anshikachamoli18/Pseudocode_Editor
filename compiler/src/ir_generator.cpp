#include "ir_generator.h"
#include <iostream>

IRGenerator::IRGenerator(const std::string& outputPath) {
    outFile.open(outputPath);
    if (!outFile) {
        std::cerr << "Error: Could not open IR output file!\n";
        exit(1);
    }
}

void IRGenerator::generate(ASTNode* root) {
    for (auto& child : root->children) {
        generateStatement(child.get());
    }
    outFile.close();
}

void IRGenerator::generateStatement(ASTNode* node) {
    if (node->type == "Assignment") {
        std::string rhs = generateExpression(node->children[1].get());
        outFile << node->children[0]->value << " = " << rhs << "\n";

    } else if (node->type == "PrintStatement") {
        std::string value = generateExpression(node->children[0].get());
        
        // [Change 1] Handle string literals for PRINT
        if (node->children[0]->type == "StringLiteral") {
            // If it's a string literal, print it with quotes
            outFile << "PRINT " << "\"" << value << "\"" << "\n";  // Ensure quotes are included
        } else {
            // If it's not a string literal, print the computed expression or variable
            outFile << "PRINT " << value << "\n";
        }
        
    }  else if (node->type == "InputStatement") {
        outFile << "READ " << node->value << "\n";

    } else if (node->type == "ReturnStatement") {
        std::string value = generateExpression(node->children[0].get());
        outFile << "RETURN " << value << "\n";

    } else if (node->type == "IfStatement") {
        handleIfElseIf(node);

    } else if (node->type == "LoopStatement") {
        std::string loopStart = "L" + std::to_string(tempVarCount++);
        std::string loopEnd = "L" + std::to_string(tempVarCount++);
        outFile << loopStart << ":\n";
        std::string cond = generateExpression(node->children[0].get());
        outFile << "IF NOT " << cond << " GOTO " << loopEnd << "\n";
        for (size_t i = 1; i < node->children.size(); ++i) {
            generateStatement(node->children[i].get());
        }
        outFile << "GOTO " << loopStart << "\n";
        outFile << loopEnd << ":\n";

    } else if (node->type == "FunctionDeclaration") {
        outFile << "FUNCTION " << node->value << ":\n";
        for (auto& stmt : node->children) {
            generateStatement(stmt.get());
        }
        outFile << "END FUNCTION\n";

    } else if (node->type == "FunctionCall") {
        std::string args;
        for (auto& arg : node->children) {
            args += generateExpression(arg.get()) + ", ";
        }
        if (!args.empty()) args.pop_back(), args.pop_back(); // remove trailing comma
        std::string temp = newTemp();
        outFile << temp << " = CALL " << node->value << "(" << args << ")\n";

    } else if (node->type == "StructDeclaration") {
        outFile << "STRUCT " << node->value << "\n";
        for (auto& field : node->children) {
            outFile << "  FIELD " << field->value << "\n";
        }
        outFile << "END STRUCT\n";

    } else if (node->type == "ArrayAccess") {
        std::string index = generateExpression(node->children[0].get());
        outFile << "ACCESS " << node->value << "[" << index << "]\n";
    }
}

void IRGenerator::handleIfElseIf(ASTNode* node) {
    std::string labelEnd = "L" + std::to_string(tempVarCount++);
    std::vector<std::string> falseLabels;

    // First handle all IfConditionBlocks
    for (auto& child : node->children) {
        if (child->type == "IfConditionBlock") {
            std::string labelNextCond = "L" + std::to_string(tempVarCount++);
            falseLabels.push_back(labelNextCond);

            // First child = condition (RelationalOperator), rest = body
            ASTNode* conditionNode = child->children[0].get();
            std::string cond = generateExpression(conditionNode);
            outFile << "IF NOT " << cond << " GOTO " << labelNextCond << "\n";

            for (size_t i = 1; i < child->children.size(); ++i) {
                generateStatement(child->children[i].get());
            }
            outFile << "GOTO " << labelEnd << "\n";
            outFile << labelNextCond << ":\n";
        }
    }

    // Then handle ElseBlock if it exists
    for (auto& child : node->children) {
        if (child->type == "ElseBlock") {
            for (auto& stmt : child->children) {
                generateStatement(stmt.get());
            }
            break; // Only one ElseBlock expected
        }
    }

    outFile << labelEnd << ":\n";
}


std::string IRGenerator::generateExpression(ASTNode* node) {
    if (node->type == "Number" || node->type == "Boolean" || node->type == "StringLiteral") {
        return node->value;  // For strings, return as-is (it will be printed correctly)
    } else if (node->type == "Variable") {
        return node->value;
    } else if (node->type == "Operator" || node->type == "RelationalOperator") {
        std::string left = generateExpression(node->children[0].get());
        std::string right = generateExpression(node->children[1].get());
        std::string temp = newTemp();
        outFile << temp << " = " << left << " " << node->value << " " << right << "\n";
        return temp;
    } else if (node->type == "FunctionCall") {
        std::string args;
        for (auto& arg : node->children) {
            args += generateExpression(arg.get()) + ", ";
        }
        if (!args.empty()) args.pop_back(), args.pop_back(); // remove trailing comma
        std::string temp = newTemp();
        outFile << temp << " = CALL " << node->value << "(" << args << ")\n";
        return temp;
    } else if (node->type == "ArrayAccess") {
        std::string index = generateExpression(node->children[0].get());
        std::string temp = newTemp();
        outFile << temp << " = " << node->value << "[" << index << "]\n";
        return temp;
    }

    return "?";
}


std::string IRGenerator::newTemp() {
    return "t" + std::to_string(tempVarCount++);
}
