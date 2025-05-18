#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>  // For input/output handling
#include <map>

class ASTNode {
public:
    std::string type;  // Node type (e.g., "Assignment", "Expression", "InputStatement")
    std::string value; // Value (e.g., variable name, operator, number)
    std::vector<std::unique_ptr<ASTNode>> children; // Child nodes

    // Constructor
    ASTNode(std::string type, std::string value) : type(type), value(value) {}

    // Add child node
    void addChild(std::unique_ptr<ASTNode> child) {
        children.push_back(std::move(child));
    }

    // Execute the node (e.g., handle input if this is an input statement)
    void execute(std::map<std::string, int>& symbolTable) {
        if (type == "InputStatement") {
            executeInput(symbolTable);
        } else {
            // Handle other types of nodes (assignments, expressions, etc.)
            std::cout << "Executing " << type << " with value: " << value << std::endl;
        }
    }

private:
    // Method to execute the input and store the value in a symbol table
    void executeInput(std::map<std::string, int>& symbolTable) {
        int value;
        std::cout << "Enter value for " << this->value << ": ";
        std::cin >> value;
        symbolTable[this->value] = value;  // Store the input value in symbol table with identifier as key
    }
};

#endif // AST_H
