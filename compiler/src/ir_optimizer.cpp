#include "ir_optimizer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <cctype>

using namespace std;

void IROptimizer::optimize(const string& inputPath, const string& outputPath) {
    vector<string> irLines = readIR(inputPath);
    vector<string> optimized = performOptimizations(irLines);
    writeIR(optimized, outputPath);
}

vector<string> IROptimizer::readIR(const string& path) {
    ifstream inFile(path);
    vector<string> lines;
    string line;
    while (getline(inFile, line)) {
        lines.push_back(line);
    }
    return lines;
}

void IROptimizer::writeIR(const vector<string>& lines, const string& path) {
    ofstream outFile(path);
    for (const auto& line : lines) {
        outFile << line << '\n';
    }
}

vector<string> IROptimizer::performOptimizations(const vector<string>& lines) {
    vector<string> optimized;

    for (const auto& line : lines) {
        if (line.find('=') != string::npos) {
            string folded = foldConstants(line);
            optimized.push_back(folded);
        } else {
            optimized.push_back(line);
        }
    }

    return optimized;
}

string IROptimizer::foldConstants(const string& line) {
    smatch match;
    // Matches: t1 = 3 + 4
    regex constExprRegex(R"(^\s*(t\d+)\s*=\s*(\d+)\s*([\+\-\*/%])\s*(\d+)\s*$)");
    if (regex_match(line, match, constExprRegex)) {
        string target = match[1];
        string left = match[2];
        string op = match[3];
        string right = match[4];

        string result = evaluateConstantExpr(left, op, right);
        return target + " = " + result;
    }

    return line;
}

string IROptimizer::evaluateConstantExpr(const string& left, const string& op, const string& right) {
    int l = stoi(left), r = stoi(right);
    int result = 0;
    if (op == "+") result = l + r;
    else if (op == "-") result = l - r;
    else if (op == "*") result = l * r;
    else if (op == "/") result = (r != 0 ? l / r : 0);  // avoid div-by-zero
    else if (op == "%") result = (r != 0 ? l % r : 0);

    return to_string(result);
}
