#pragma once
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "ISA.h"

struct ProgramLayout {
    std::vector<std::string> text;
    std::vector<int> data;
    std::unordered_map<std::string, uint16_t> labels;
};

template <InstructionSet ISA>
class Assembler {
   public:
    static ProgramLayout parseProgramLayout(const std::string& filename) {
        std::ifstream infile(filename);
        ProgramLayout prog;

        std::string line;
        uint16_t address = 0;
        bool inText = false;
        bool inData = false;

        while (std::getline(infile, line)) {
            line = trim(line);

            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
                line = trim(line);
            }

            if (line.empty()) continue;

            if (line == ".text") {
                inText = true;
                inData = false;
                continue;
            }
            if (line == ".data") {
                inText = false;
                inData = true;
                continue;
            }

            if (line.ends_with(":")) {
                std::string label = line.substr(0, line.size() - 1);
                prog.labels[label] = address;
                continue;
            }

            if (inText) {
                prog.text.push_back(line);
                address++;
            } else if (inData) {
                auto parts = split(line);
                if (parts[0].ends_with(":")) {
                    std::string label = parts[0];
                    label.pop_back();
                    prog.labels[label] = address;
                    if (parts.size() > 1) {
                        prog.data.push_back(std::stoi(parts[1]));
                        address++;
                    }
                } else {
                    prog.data.push_back(std::stoi(parts[0]));
                    address++;
                }
            }
        }

        return prog;
    }

    static std::vector<uint16_t> assemble(const std::string& filename) {
        auto prog = parseProgramLayout(filename);

        std::vector<uint16_t> machineCode;
        machineCode.reserve(prog.text.size() + prog.data.size());

        for (const auto& instr : prog.text) {
            auto items = split(instr);
            auto opcode = opcodeMap.at(items[0]);

            if (items.size() == 2) {
                int value = isNumber(items[1]) ? std::stoi(items[1]) : prog.labels.at(items[1]);

                machineCode.push_back(opcode << 8 | value);
            } else {
                machineCode.push_back(opcode << 8);
            }
        }

        for (int d : prog.data) {
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