#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<std::string> split(const std::string& s, char delimiter = ' ') {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

// clang-format off
std::unordered_map<std::string, uint8_t> opcodeMap = {
    {"add", 0x00},
    {"sub", 0x01},
    {"mul", 0x02},
    {"adda", 0x03},
    {"suba", 0x04},
    {"addx", 0x05},
    {"subx", 0x06},
    {"ld", 0x07},
    {"st", 0x08},
    {"lda", 0x09},
    {"sta", 0x0A},
    {"ldi", 0x0B},
    {"sti", 0x0C},
    {"br", 0x0D},
    {"brz", 0x0E},
    {"brnz", 0x0F},
    {"shl", 0x10},
    {"shr", 0x11},
    {"stop", 0x13},
};

// clang-format on

int main() {
    std::filesystem::path file{"code.s"};
    std::ifstream infile(file);

    std::vector<std::string> text;
    std::vector<int> data;
    std::unordered_map<std::string, int> labels;

    std::vector<uint16_t> machineCode;

    std::string line;

    size_t codeSize = 0;

    // First pass: parse labels
    while (std::getline(infile, line)) {
        if (line.size() == 0 || line.starts_with("#")) {
            continue;
        }

        if (line == ".text") {
            continue;
        }

        if (line == ".data") {
            break;
        }

        if (line.ends_with(":")) {
            line.pop_back(); // remove ":"
            labels[line] = codeSize;
            continue;
        }

        text.push_back(line);

        ++codeSize;
    }

    // First pass: parse data
    while (std::getline(infile, line)) {
        if (line.size() == 0 || line.starts_with("#")) {
            continue;
        }

        const auto items = split(line);
        std::string label = items[0];
        label.pop_back(); // remove ":"

        int value = std::stoi(items[1]);
        labels[label] = codeSize;
        data.push_back(value);

        ++codeSize;
    }

    machineCode.reserve(text.size() + data.size());

    // Second pass: replace labels with addresses
    for (size_t i = 0; i < text.size(); ++i) {
        std::string instruction = text[i];
        const auto items = split(instruction);
        auto opcode = opcodeMap[items[0]];
        if (items.size() == 2) {
            auto label = items[1];
            auto value = labels[label];
            machineCode.push_back(opcode << 8 | value);
        } else {
            machineCode.push_back(opcode << 8);
        }
    }

    // Second pass: add remaining data
    for (auto d : data) {
        machineCode.push_back(d);
    }

    for (auto l : machineCode) {
        std::cout << std::hex << l << '\n';
    }
}