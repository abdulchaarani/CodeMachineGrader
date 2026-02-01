#include <string>
#include <unordered_map>
#include <vector>

#include "Cpu.h"
#include "ISA.h"

std::vector<std::string> split(const std::string& s, char delimiter = ' ') {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filepath>\n";
        return 1;
    }

    const std::string codePath{argv[1]};

    if (!std::filesystem::exists(codePath)) {
        std::cerr << "Error: File '" << codePath << "' does not exist\n";
        return 1;
    }

    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);
    cpu.runProgram();
}