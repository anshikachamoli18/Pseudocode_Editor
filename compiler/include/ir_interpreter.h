#ifndef IR_INTERPRETER_H
#define IR_INTERPRETER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

class IRInterpreter {
public:
    void interpret(const std::string& path);

private:
    struct Frame {
        std::unordered_map<std::string, int> variables;
        std::string returnTarget;
        int returnAddress = -1;
    };

    std::vector<std::string> readIR(const std::string& path);
    void preprocess(const std::vector<std::string>& lines);
    void execute();
    void executeLine(const std::string& line);
    int evaluateOperand(const std::string& token);
    void jumpToLabel(const std::string& label);
    void callFunction(const std::string& name, const std::vector<std::string>& args, const std::string& target);

    std::vector<std::string> irCode;
    std::unordered_map<std::string, int> labelMap;
    std::unordered_map<std::string, int> functionMap;
    std::unordered_map<std::string, std::vector<int>> arrays;
    std::unordered_map<std::string, std::unordered_map<std::string, int>> structs;

    std::stack<Frame> callStack;
    int instructionPointer = 0;
};

#endif
