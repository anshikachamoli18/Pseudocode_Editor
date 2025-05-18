#include "../include/parser.h"
#include "../include/token.h"
#include <iostream>
#include <memory>
#include "../include/ast.h"
#include <vector>

Parser::Parser(std::vector<Token> tokens) : tokens(tokens), currentPos(0) {}

Token Parser::currentToken() {
    return (currentPos < tokens.size()) ? tokens[currentPos] : Token{TokenType::END_OF_FILE, "EOF"};
}

void Parser::advance() {
    if (currentPos < tokens.size()) {
        currentPos++;
    }
}

std::unique_ptr<ASTNode> Parser::parse() {
    auto root = std::make_unique<ASTNode>("Program", "");

    if (currentToken().value == "START") {
        advance();  // Skip "START"
    }

    while (currentToken().type != TokenType::END_OF_FILE && currentToken().value != "END") {
        auto stmt = parseStatement();
        if (stmt) {
            root->children.push_back(std::move(stmt));
        } else {
            std::cerr << "Error: Failed to parse statement at token '" << currentToken().value << "'\n";
            advance();  // Prevent infinite loop
        }
    }

    if (currentToken().value == "END") {
        advance(); // Good
    } else if (currentToken().type != TokenType::END_OF_FILE) {
        std::cerr << "Error: Missing 'END' keyword.\n";
    }
    

    return root;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (currentToken().value == "READ") {
        return parseInputStatement();
    }

    if (currentToken().value == "RETURN") {
        advance(); // Consume "RETURN"

        auto returnNode = std::make_unique<ASTNode>("ReturnStatement", "RETURN");
        returnNode->children.push_back(parseExpression());

        return returnNode;
    }

    if (currentToken().type == TokenType::IDENTIFIER && currentPos + 1 < tokens.size() && tokens[currentPos + 1].value == "=") {
        return parseAssignment();
    } else if (currentToken().value == "PRINT") {
        return parsePrintStatement();
    } else if (currentToken().value == "IF") {
        return parseIfStatement();
    } else if (currentToken().value == "WHILE" || currentToken().value == "FOR") {
        return parseLoopStatement();
    } else if (currentToken().value == "FUNCTION") {
        return parseFunctionDeclaration();
    } else if (currentToken().value == "STRUCT") {
        return parseStructDeclaration();
    } else {
        std::cerr << "Error: Unexpected token '" << currentToken().value << "'\n";
        return nullptr;
    }
}

std::unique_ptr<ASTNode> Parser::parseInputStatement() {
    expect("READ");  // Ensure we have the "READ" keyword

    Token identifier = currentToken();
    if (identifier.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected an identifier after 'READ'");
    }
    advance(); // Consume the identifier

    // Create and return the AST node for the input statement
    return std::make_unique<ASTNode>("InputStatement", identifier.value);  
}


std::unique_ptr<ASTNode> Parser::parseAssignment() {
    Token varName = currentToken();
    advance(); // Move past variable
    advance(); // Move past '='

    auto assignmentNode = std::make_unique<ASTNode>("Assignment", "=");
    assignmentNode->children.push_back(std::make_unique<ASTNode>("Variable", varName.value));
    assignmentNode->children.push_back(parseExpression());

    return assignmentNode;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseRelationalExpression();
}

// Parses relational operators (>, <, >=, <=, ==, !=) which have lower precedence than arithmetic
std::unique_ptr<ASTNode> Parser::parseRelationalExpression() {
    auto left = parseArithmeticExpression();  // First, parse arithmetic expression

    while (currentToken().value == ">" || currentToken().value == "<" ||
           currentToken().value == ">=" || currentToken().value == "<=" ||
           currentToken().value == "==" || currentToken().value == "!=") {
        std::string op = currentToken().value;
        advance();
        auto node = std::make_unique<ASTNode>("RelationalOperator", op);
        node->children.push_back(std::move(left));
        node->children.push_back(parseArithmeticExpression());  // Next arithmetic expression
        left = std::move(node);
    }
    return left;
}

// Parses addition and subtraction (+, -)
std::unique_ptr<ASTNode> Parser::parseArithmeticExpression() {
    auto left = parseTerm();

    while (currentToken().value == "+" || currentToken().value == "-") {
        std::string op = currentToken().value;
        advance();
        auto node = std::make_unique<ASTNode>("Operator", op);
        node->children.push_back(std::move(left));
        node->children.push_back(parseTerm());
        left = std::move(node);
    }
    return left;
}

// Parses multiplication and division (*, /)
std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();

    while (currentToken().value == "*" || currentToken().value == "/"||currentToken().value == "%" ) {
        std::string op = currentToken().value;
        advance();
        auto node = std::make_unique<ASTNode>("Operator", op);
        node->children.push_back(std::move(left));
        node->children.push_back(parseFactor());
        left = std::move(node);
    }
    return left;
}

// Parses individual elements (numbers, variables, function calls, array accesses, parentheses)
std::unique_ptr<ASTNode> Parser::parseFactor() {
    Token token = currentToken();

    if (token.type == TokenType::INTEGER_LITERAL || token.type == TokenType::FLOAT_LITERAL) {
        advance();
        return std::make_unique<ASTNode>("Number", token.value);
    } 
    else if (token.type == TokenType::BOOLEAN_LITERAL) {
        advance();
        return std::make_unique<ASTNode>("Boolean", token.value);
    }
    else if (token.type == TokenType::IDENTIFIER) {
        if (tokens[currentPos + 1].value == "(") {
            return parseFunctionCall();
        } else if (tokens[currentPos + 1].value == "[") {
            return parseArrayAccess();
        }
        advance();
        return std::make_unique<ASTNode>("Variable", token.value);
    } 
    else if (token.value == "(") {
        advance();
        auto expr = parseExpression();
        if (currentToken().value == ")") {
            advance();
        } else {
            std::cerr << "Error: Expected closing parenthesis.\n";
        }
        return expr;
    }

    std::cerr << "Error: Unexpected token '" << token.value << "' in expression.\n";
    return nullptr;
}


std::unique_ptr<ASTNode> Parser::parsePrintStatement() {
    advance(); // Skip "PRINT"
    
    auto printNode = std::make_unique<ASTNode>("PrintStatement", "PRINT");

    // Check if the next token is a STRING_LITERAL
    if (currentToken().type == TokenType::STRING) {
        printNode->children.push_back(std::make_unique<ASTNode>("StringLiteral", currentToken().value));
        advance();  // Consume the string literal
    } else {
        printNode->children.push_back(parseExpression()); // Handle expressions as usual
    }

    return printNode;
}

Token Parser::peek() {
    if (currentPos + 1 < tokens.size()) {
        return tokens[currentPos + 1];
    }
    return Token{TokenType::END_OF_FILE, "EOF"};
}

Token Parser::previousToken() {
    if (currentPos > 0) {
        return tokens[currentPos - 1];
    }
    return Token{TokenType::INVALID, ""};
}


void Parser::parseConditionAndBlock(std::unique_ptr<ASTNode>& ifNode) {
    auto condBlock = std::make_unique<ASTNode>("IfConditionBlock", "ConditionBlock");

    // Parse condition
    auto condition = parseExpression();
    condBlock->children.push_back(std::move(condition));

    // Expect THEN
    expect("THEN");

    // Parse statements until we hit ELSE, ELSE IF, or ENDIF
    while (currentToken().value != "ELSE" &&
           !(currentToken().value == "IF" && previousToken().value == "ELSE") &&
           currentToken().value != "ENDIF" &&
           currentToken().type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) {
            condBlock->children.push_back(std::move(stmt));
        } else {
            advance(); // Skip unknown tokens
        }
    }

    // Add this block to the parent IfStatement
    ifNode->children.push_back(std::move(condBlock));
}



std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    // Consume 'IF'
    advance();

    // Root node for the entire if-else chain
    auto ifNode = std::make_unique<ASTNode>("IfStatement", "IF");

    // Handle initial IF block
    parseConditionAndBlock(ifNode);

    // Handle any ELSE IF blocks
    while (currentToken().value == "ELSE" && peek().value == "IF") {
        advance(); // consume 'ELSE'
        advance(); // consume 'IF'
        parseConditionAndBlock(ifNode); // treat as another condition-block
    }

    // Handle optional ELSE block
    if (currentToken().value == "ELSE") {
        advance(); // consume 'ELSE'
        auto elseBlock = std::make_unique<ASTNode>("ElseBlock", "ELSE");

        // Parse statements in the ELSE block
        while (currentToken().value != "ENDIF" &&
               currentToken().type != TokenType::END_OF_FILE) {
            auto stmt = parseStatement();
            if (stmt) {
                elseBlock->children.push_back(std::move(stmt));
            } else {
                advance(); // Skip invalid tokens
            }
        }

        ifNode->children.push_back(std::move(elseBlock));
    }

    // Expect ENDIF
    expect("ENDIF");

    return ifNode;
}





std::unique_ptr<ASTNode> Parser::parseLoopStatement() {
    Token loopToken = currentToken();
    advance();

    auto loopNode = std::make_unique<ASTNode>("LoopStatement", loopToken.value);
    
    if (loopToken.value == "FOR") {
        auto init = parseAssignment();
        loopNode->children.push_back(std::move(init));
        
        expect("TO");
        loopNode->children.push_back(parseExpression());

        if (currentToken().value == "STEP") {
            advance();
            loopNode->children.push_back(parseExpression());
        }
    } else {
        loopNode->children.push_back(parseExpression());
    }

    expect("DO");

    while (currentToken().value != "ENDWHILE" && currentToken().value != "ENDFOR" && currentToken().type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) {
            loopNode->children.push_back(std::move(stmt));
        } else {
            std::cerr << "Error: Invalid statement inside loop.\n";
            advance();
        }
    }

    if (currentToken().value == "ENDWHILE" || currentToken().value == "ENDFOR") {
        advance();
    } else {
        std::cerr << "Error: Missing 'ENDWHILE' or 'ENDFOR' keyword in loop.\n";
    }

    return loopNode;
}


std::vector<std::string> Parser::parseParameterList() {
    std::vector<std::string> parameters;

    // If no parameters (empty parentheses), return empty list
    if (currentToken().value == ")") {
        return parameters;
    }

    // Parse parameters
    while (true) {
        parameters.push_back(currentToken().value);
        advance();  // Move past parameter name

        if (currentToken().value == ")") {
            break;  // End of parameter list
        }

        expect(",");  // Ensure there's a comma between parameters
    }

    return parameters;
}

void Parser::expect(const std::string& expectedValue) {
    if (currentToken().value != expectedValue) {
        std::cerr << "Error: Expected '" << expectedValue << "' but got '" << currentToken().value << "'\n";
        exit(EXIT_FAILURE);
    }
    advance();  // Move to the next token
}

std::unique_ptr<ASTNode> Parser::parseFunctionDeclaration() {
    expect("FUNCTION");  // Ensure FUNCTION keyword
    std::string functionName = currentToken().value;
    advance();  // Move past function name

    expect("(");
    std::vector<std::string> parameters = parseParameterList();
    expect(")");  // Ensure closing parenthesis

    auto funcNode = std::make_unique<ASTNode>("FunctionDeclaration", functionName);

    // Add parameters as child nodes
    for (const auto& param : parameters) {
        funcNode->children.push_back(std::make_unique<ASTNode>("Parameter", param));
    }

    // Parse function body
    while (currentToken().value != "ENDFUNCTION" && currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().value == "RETURN") {
            advance();  // Skip "RETURN"

            // Create a ReturnStatement node
            auto returnNode = std::make_unique<ASTNode>("ReturnStatement", "RETURN");

            // Parse the return expression
            returnNode->children.push_back(parseExpression());

            // Add return node to function body
            funcNode->children.push_back(std::move(returnNode));
        } else {
            auto stmt = parseStatement();
            if (stmt) {
                funcNode->children.push_back(std::move(stmt));
            } else {
                std::cerr << "Error: Invalid statement inside function.\n";
                advance();  // Skip to continue parsing
            }
        }
    }

    expect("ENDFUNCTION");  // Ensure function properly ends
    return funcNode;
}



std::unique_ptr<ASTNode> Parser::parseStructDeclaration() {
    advance(); // Move past "STRUCT"

    if (currentToken().type == TokenType::IDENTIFIER) {
        auto structNode = std::make_unique<ASTNode>("StructDeclaration", currentToken().value);
        advance(); // Move past struct name

        if (currentToken().value == "{") {
            advance(); // Move past '{'

            while (currentToken().type == TokenType::IDENTIFIER) {
                auto fieldNode = std::make_unique<ASTNode>("Field", currentToken().value);
                advance();
                structNode->children.push_back(std::move(fieldNode));
                if (currentToken().value == ";") {
                    advance(); // Move past ';'
                } else {
                    std::cerr << "Error: Expected ';' after struct field.\n";
                }
            }

            if (currentToken().value == "}") {
                advance(); // Move past '}'
            } else {
                std::cerr << "Error: Expected '}' at the end of struct declaration.\n";
            }
        } else {
            std::cerr << "Error: Expected '{' after struct name.\n";
        }

        return structNode;
    }

    std::cerr << "Error: Expected struct name.\n";
    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseArrayAccess() {
    auto arrayNode = std::make_unique<ASTNode>("ArrayAccess", currentToken().value);
    advance(); // Move past array name

    if (currentToken().value == "[") {
        advance(); // Move past '['
        arrayNode->children.push_back(parseExpression());

        if (currentToken().value == "]") {
            advance(); // Move past ']'
        } else {
            std::cerr << "Error: Expected ']' after array index.\n";
        }
    } else {
        std::cerr << "Error: Expected '[' for array access.\n";
    }

    return arrayNode;
}

std::unique_ptr<ASTNode> Parser::parseFunctionCall() {
    Token funcName = currentToken();
    advance(); // Move past function name

    auto funcNode = std::make_unique<ASTNode>("FunctionCall", funcName.value);

    if (currentToken().value == "(") {
        advance(); // Move past '('
        
        while (currentToken().value != ")" && currentToken().type != TokenType::END_OF_FILE) {
            funcNode->children.push_back(parseExpression());
            if (currentToken().value == ",") advance();
        }

        if (currentToken().value == ")") {
            advance(); // Move past ')'
        } else {
            std::cerr << "Error: Expected ')' after function arguments.\n";
        }
    } else {
        std::cerr << "Error: Expected '(' after function name.\n";
    }

    return funcNode;
}
