#ifndef IR_OPTIMIZER_H
#define IR_OPTIMIZER_H

#include <string>
#include <vector>

class IROptimizer {
public:
    void optimize(const std::string& inputPath, const std::string& outputPath);

private:
    std::vector<std::string> readIR(const std::string& path);
    void writeIR(const std::vector<std::string>& lines, const std::string& path);
    std::vector<std::string> performOptimizations(const std::vector<std::string>& lines);

    std::string foldConstants(const std::string& line);
    bool isConstantExpression(const std::string& expr);
    std::string evaluateConstantExpr(const std::string& left, const std::string& op, const std::string& right);
};

#endif // IR_OPTIMIZER_H
