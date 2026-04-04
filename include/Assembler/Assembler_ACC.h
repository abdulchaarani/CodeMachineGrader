#pragma once
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Assembler/ProgramLayout.h"
#include "Assembler/ProgramLayoutParser.h"
#include "architectures.h"

template <AccLike ISA>
class Assembler_ACC {
   public:
    using Layout = ProgramLayoutAcc;

    static Layout parseProgramLayout(const std::string& filename) {
        return ProgramLayoutParser::parse<Layout>(filename);
    }

    static std::vector<uint16_t> assemble(const std::string& filename) {
        return assemble(parseProgramLayout(filename));
    }

    static std::vector<uint16_t> assemble(const Layout& prog) {
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

        for (int16_t d : prog.data) machineCode.push_back(static_cast<uint16_t>(d));

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
