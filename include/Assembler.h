#pragma once
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "ISA.h"

template <InstructionSet ISA>
class Assembler {
   public:
    static std::vector<uint16_t> assemble(const std::string& filename) {
        std::filesystem::path file{filename};
        std::ifstream infile(file);
        std::vector<std::string> text;
        std::vector<int> data;
        std::unordered_map<std::string, int> labels;
        std::vector<uint16_t> machineCode;
        std::string line;
        size_t codeSize = 0;

        // First pass: parse labels and text section
        while (std::getline(infile, line)) {
            line = trim(line);
            // Remove comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
                line = trim(line);
            }

            if (line.empty()) {
                continue;
            }
            if (line == ".text") {
                continue;
            }
            if (line == ".data") {
                break;
            }
            if (line.ends_with(":")) {
                line.pop_back();  // remove ":"
                labels[line] = codeSize;
                continue;
            }
            text.push_back(line);
            ++codeSize;
        }

        // First pass: parse data section
        while (std::getline(infile, line)) {
            line = trim(line);
            // Remove comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
                line = trim(line);
            }

            if (line.empty()) {
                continue;
            }

            const auto items = split(line);

            // Check if line has a label (contains ':')
            if (items[0].ends_with(":")) {
                std::string label = items[0];
                label.pop_back();  // remove ":"
                labels[label] = codeSize;

                // Check if there's a value after the label
                if (items.size() > 1) {
                    int value = std::stoi(items[1]);
                    data.push_back(value);
                    ++codeSize;
                }
            } else {
                // No label, just a literal value
                int value = std::stoi(items[0]);
                data.push_back(value);
                ++codeSize;
            }
        }

        machineCode.reserve(text.size() + data.size());

        // Second pass: replace labels with addresses or use literal values
        for (size_t i = 0; i < text.size(); ++i) {
            std::string instruction = text[i];
            const auto items = split(instruction);
            auto opcode = opcodeMap[items[0]];

            if (opcodeMap.find(items[0]) == opcodeMap.end()) {
                throw std::runtime_error("Invalid instruction: " + items[0]);
            }

            if (items.size() == 2) {
                std::string operand = items[1];
                int value;

                // Check if operand is a number (literal) or a label
                if (isNumber(operand)) {
                    value = std::stoi(operand);
                } else {
                    // It's a label
                    value = labels[operand];
                }

                machineCode.push_back(opcode << 8 | value);
            } else {
                machineCode.push_back(opcode << 8);
            }
        }

        // Second pass: add remaining data
        for (auto d : data) {
            machineCode.push_back(d);
        }

        return machineCode;
    }

   private:
    static bool isNumber(const std::string& s) {
        if (s.empty()) return false;

        size_t start = 0;
        // Handle negative numbers
        if (s[0] == '-' || s[0] == '+') {
            if (s.length() == 1) return false;
            start = 1;
        }

        for (size_t i = start; i < s.length(); ++i) {
            if (!std::isdigit(s[i])) {
                return false;
            }
        }
        return true;
    }

    static std::vector<std::string> split(const std::string& s, char delimiter = ' ') {
        std::vector<std::string> result;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            item = trim(item);
            if (!item.empty()) {
                result.push_back(item);
            }
        }
        return result;
    }

    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r\f\v");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r\f\v");
        return str.substr(first, (last - first + 1));
    }

    inline static std::unordered_map<std::string, uint8_t> opcodeMap = [] {
        std::unordered_map<std::string, uint8_t> map;
        for (auto& [name, code] : ISA::opcodes) map[name] = code;
        return map;
    }();
};