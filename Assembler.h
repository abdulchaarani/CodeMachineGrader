#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
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

        // First pass: parse labels
        while (std::getline(infile, line)) {
            line = trim(line);
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
                line.pop_back();  // remove ":"
                labels[line] = codeSize;
                continue;
            }

            text.push_back(line);

            ++codeSize;
        }

        // First pass: parse data
        while (std::getline(infile, line)) {
            line = trim(line);
            if (line.size() == 0 || line.starts_with("#")) {
                continue;
            }

            const auto items = split(line);
            std::string label = items[0];
            label.pop_back();  // remove ":"

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

        return machineCode;
    }

   private:
    static std::vector<std::string> split(const std::string& s, char delimiter = ' ') {
        std::vector<std::string> result;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
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
