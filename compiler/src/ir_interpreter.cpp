#include "ir_interpreter.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <algorithm>

using namespace std;

vector<string> IRInterpreter::readIR(const string& path) {
    ifstream file(path);
    vector<string> lines;
    string line;
    while (getline(file, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

void emitJSON(const std::string& type, const std::string& key, const std::string& val) {
    std::cout << "{\"type\":\"" << type << "\",\"" << key << "\":\"" << val << "\"}" << std::endl;
}

void IRInterpreter::interpret(const string& path) {
    irCode = readIR(path);
    preprocess(irCode);
    callStack.push(Frame());  // main frame
    execute();
}

void IRInterpreter::preprocess(const vector<string>& lines) {
    for (int i = 0; i < lines.size(); ++i) {
        smatch m;
        if (regex_match(lines[i], m, regex(R"(^(\w+):$)")))
            labelMap[m[1]] = i;
        else if (regex_match(lines[i], m, regex(R"(FUNCTION\s+(\w+):)")))
            functionMap[m[1]] = i + 1;
    }
}

void IRInterpreter::execute() {
    while (instructionPointer < irCode.size()) {
        executeLine(irCode[instructionPointer]);
        instructionPointer++;
    }
}

int IRInterpreter::evaluateOperand(const string& token) {
    if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1))
        return stoi(token);
    if (callStack.top().variables.count(token))
        return callStack.top().variables[token];
    return 0;
}

void IRInterpreter::jumpToLabel(const string& label) {
    if (labelMap.count(label))
        instructionPointer = labelMap[label] - 1;
    else
        cerr << "[ERROR] Label not found: " << label << endl;
}

void IRInterpreter::executeLine(const string& line) {
    smatch m;

    if (regex_match(line, regex(R"(^\w+:$)"))) return;
    if (regex_match(line, regex(R"(^FUNCTION\s+\w+:$)"))) return;

    if (regex_match(line, m, regex(R"(^(\w+)\s*=\s*(\w+|\d+)\s*(==|!=|>=|<=|>|<)\s*(\w+|\d+)$)"))) {
        int a = evaluateOperand(m[2]);
        int b = evaluateOperand(m[4]);
        string op = m[3];
        bool result = false;

        if (op == "==") result = (a == b);
        else if (op == "!=") result = (a != b);
        else if (op == ">")  result = (a > b);
        else if (op == "<")  result = (a < b);
        else if (op == ">=") result = (a >= b);
        else if (op == "<=") result = (a <= b);

        callStack.top().variables[m[1]] = result ? 1 : 0;
    }
    else if (regex_match(line, m, regex(R"(^(\w+)\s*=\s*(\w+|\d+)\s*([\+\-\*/%])\s*(\w+|\d+)$)"))) {
        int a = evaluateOperand(m[2]);
        int b = evaluateOperand(m[4]);
        string op = m[3];
        if (op == "+") callStack.top().variables[m[1]] = a + b;
        else if (op == "-") callStack.top().variables[m[1]] = a - b;
        else if (op == "*") callStack.top().variables[m[1]] = a * b;
        else if (op == "/") callStack.top().variables[m[1]] = b != 0 ? a / b : 0;
        else if (op == "%") callStack.top().variables[m[1]] = b != 0 ? a % b : 0;
    }
    else if (regex_match(line, m, regex(R"(^(\w+)\s*=\s*(\w+|\d+)$)"))) {
        callStack.top().variables[m[1]] = evaluateOperand(m[2]);
    }
    else if (regex_match(line, m, regex(R"(^PRINT\s+(.+)$)"))) {
        string arg = m[1];
        if (arg.front() == '"' && arg.back() == '"') {
            emitJSON("output", "message", arg.substr(1, arg.length() - 2));
        } else {
            int val = evaluateOperand(arg);
            emitJSON("output", "message", arg + " = " + to_string(val));
        }
    }
    else if (regex_match(line, m, regex(R"(^READ\s+(\w+)$)"))) {
        emitJSON("input", "prompt", m[1]);
        string inputVal;
        getline(cin, inputVal);
        callStack.top().variables[m[1]] = stoi(inputVal);
    }
    else if (regex_match(line, m, regex(R"(^IF\s+NOT\s+(\w+)\s+GOTO\s+(\w+)$)"))) {
        int cond = evaluateOperand(m[1]);
        if (!cond) jumpToLabel(m[2]);
    }
    else if (regex_match(line, m, regex(R"(^GOTO\s+(\w+)$)"))) {
        jumpToLabel(m[1]);
    }
    else if (regex_match(line, m, regex(R"(^RETURN\s+(\w+)$)"))) {
        int value = evaluateOperand(m[1]);
        string retTarget = callStack.top().returnTarget;
        callStack.pop();
        if (!callStack.empty()) {
            callStack.top().variables[retTarget] = value;
            emitJSON("output", "message", "Returned: " + to_string(value));
        }
        while (instructionPointer < irCode.size() &&
               !regex_match(irCode[instructionPointer], regex(R"(^END FUNCTION$)")))
            instructionPointer++;
    }
    else if (regex_match(line, m, regex(R"(^(\w+)\s*=\s*CALL\s+(\w+)\((.*)\)$)"))) {
        string target = m[1], funcName = m[2], argStr = m[3];
        vector<string> args;
        istringstream ss(argStr);
        string tok;
        while (getline(ss, tok, ',')) {
            tok.erase(remove_if(tok.begin(), tok.end(), ::isspace), tok.end());
            if (!tok.empty()) args.push_back(tok);
        }
        callFunction(funcName, args, target);
    }
    else if (regex_match(line, m, regex(R"(^ACCESS\s+(\w+)\[(\w+)\]$)"))) {
        string arr = m[1];
        int idx = evaluateOperand(m[2]);
        emitJSON("output", "message", arr + "[" + to_string(idx) + "] = " + to_string(arrays[arr][idx]));
    }
    else {
        cerr << "[WARNING] Unknown instruction: " << line << endl;
    }
}

void IRInterpreter::callFunction(const string& name, const vector<string>& args, const string& target) {
    if (!functionMap.count(name)) {
        cerr << "[ERROR] Function not found: " << name << endl;
        return;
    }

    Frame newFrame;
    newFrame.returnTarget = target;

    int funcLine = functionMap[name];
    for (size_t i = 0; i < args.size(); ++i) {
        newFrame.variables["arg" + to_string(i)] = evaluateOperand(args[i]);
    }

    callStack.push(newFrame);
    instructionPointer = functionMap[name] - 1;
}
