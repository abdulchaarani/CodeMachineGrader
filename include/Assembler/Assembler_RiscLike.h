#pragma once
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "architectures.h"

struct ProgramLayoutRisc {
    std::vector<std::string> text;
    std::vector<int32_t> data;
    std::unordered_map<std::string, uint32_t> textLabels;
    std::unordered_map<std::string, uint32_t> dataLabels;
};

template <RiscLike ISA>
class Assembler_Risc {
   public:
    static ProgramLayoutRisc parseProgramLayout(const std::string& filename) {
        std::ifstream infile(filename);
        if (!infile) throw std::runtime_error("Cannot open file: " + filename);

        ProgramLayoutRisc prog;
        std::string line;

        uint32_t textAddress = 0;
        uint32_t dataAddress = 0;

        bool inText = false, inData = false;

        while (std::getline(infile, line)) {
            line = stripComment(trim(line));
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
                prog.textLabels[line.substr(0, line.size() - 1)] = textAddress;
                continue;
            }

            if (inText) {
                prog.text.push_back(line);
                textAddress++;
            } else if (inData) {
                auto parts = tokenise(line);
                if (parts[0].ends_with(":")) {
                    std::string label = parts[0].substr(0, parts[0].size() - 1);
                    prog.dataLabels[label] = dataAddress;
                    if (parts.size() > 1) {
                        prog.data.push_back(std::stoi(parts[1]));
                        dataAddress++;
                    }
                } else {
                    prog.data.push_back(std::stoi(parts[0]));
                    dataAddress++;
                }
            }
        }
        return prog;
    }

    static std::vector<uint32_t> assemble(const std::string& filename) {
        return assemble(parseProgramLayout(filename));
    }

    static std::vector<uint32_t> assemble(const ProgramLayoutRisc& prog) {
        std::vector<uint32_t> machineCode;
        machineCode.reserve(prog.text.size() + prog.data.size());
        for (const auto& line : prog.text)
            machineCode.push_back(encodeLine(line, prog.textLabels, prog.dataLabels));
        for (int32_t d : prog.data) machineCode.push_back(static_cast<uint32_t>(d));
        return machineCode;
    }

   private:
    static uint32_t encodeLine(const std::string& line,
                               const std::unordered_map<std::string, uint32_t>& textLabels,
                               const std::unordered_map<std::string, uint32_t>& dataLabels) {
        auto tokens = tokenise(line);
        const std::string& mnemonic = tokens[0];

        auto it = ISA::instrMap.find(mnemonic);
        if (it == ISA::instrMap.end()) throw std::runtime_error("Unknown mnemonic: " + mnemonic);

        const typename ISA::InstructionDescription& desc = it->second;
        uint32_t word = 0;

        switch (desc.format) {
            // ---------------
            // alu bits:
            // 27    = 0
            // 26-24 = op_alu
            // 20-16 = rdst
            // 12-8  = rsrc1
            // [5-0  = rsrc2]
            // ---------------
            case ISA::Opcode::ALU: {
                uint32_t op = desc.opcode & 0x7;  // 3 bits
                uint32_t dst = parseReg(tokens.at(1));
                uint32_t s1 = parseReg(tokens.at(2));

                word = (0 << 27);     // bit 27 = 0
                word |= (op << 24);   // bits 26-24
                word |= (dst << 16);  // bits 20-16
                word |= (s1 << 8);    // bits 12-8

                // 3 registres instruction
                if (tokens.size() > 3) {
                    uint32_t s2 = parseReg(tokens.at(3));
                    word |= (s2 << 0);  // bits 5-0
                }
                break;
            }

            // ---------------
            // read_mum bits:
            // 27-24 = 1000
            // 20-16 = rdst
            // 12-8  = rsrc1
            // ---------------
            case ISA::Opcode::READ_MEM: {
                uint32_t dst = parseReg(tokens.at(1));
                uint32_t s1 = parseReg(tokens.at(2));

                word = (static_cast<uint32_t>(ISA::Opcode::READ_MEM) << 24);  // bits 27-24 = 1000
                word |= (dst << 16);
                word |= (s1 << 8);
                break;
            }

            // ---------------
            // write_mem bits:
            // 27-24 = 1001
            // 12-8  = rsrc1
            // 5-0   = rsrc2
            // ---------------
            case ISA::Opcode::WRITE_MEM: {
                uint32_t s1 = parseReg(tokens.at(1));
                uint32_t s2 = parseReg(tokens.at(2));

                word = (static_cast<uint32_t>(ISA::Opcode::WRITE_MEM) << 24);  // bits 27-24 = 1001
                word |= (s1 << 8);
                word |= (s2 << 0);
                break;
            }

            // ---------------
            // ldi bits:
            // 27-24 = 1010
            // 20-16 = rdst
            // 15-0  = disp
            // ---------------
            case ISA::Opcode::LDI: {
                uint32_t dst = parseReg(tokens.at(1));
                int32_t imm = parseImm(tokens.at(2), dataLabels);

                if (imm < -32768 || imm > 65535)
                    throw std::runtime_error("ldi immediate out of 16-bit range");

                word = (static_cast<uint32_t>(ISA::Opcode::LDI) << 24);  // bits 27-24 = 1010
                word |= (dst << 16);
                word |= (static_cast<uint32_t>(imm) & 0xFFFF);
                break;
            }

            // ---------------
            // jump bits:
            // 27-24 = 1100
            // 19-16 = jtype,
            // 11-0  = disp (address)
            // ---------------
            case ISA::Opcode::JUMP: {
                uint32_t jtype = desc.opcode & 0xF;  // 4 bits
                uint32_t addr = resolveLabel(tokens.at(1), textLabels);

                if (addr > 0xFFF)
                    throw std::runtime_error("Branch target address exceeds 12-bit range: " +
                                             tokens.at(1));

                word = (static_cast<uint32_t>(ISA::Opcode::JUMP) << 24);  // bits 27-24 = 1100
                word |= (jtype << 16);                                    // bits 19-16
                word |= (addr & 0xFFF);                                   // bits 11-0
                break;
            }

            // ---------------
            // stop: 27-24=1111
            // ---------------
            case ISA::Opcode::STOP: {
                word = (static_cast<uint32_t>(ISA::Opcode::STOP) << 24);
                break;
            }
        }

        return word;
    }

    static uint32_t parseReg(const std::string& s) {
        std::string r = s;
        if (!r.empty() && r.front() == '(') r = r.substr(1);
        if (!r.empty() && r.back() == ')') r = r.substr(0, r.size() - 1);

        if (r.size() < 2 || (r[0] != 'r' && r[0] != 'R'))
            throw std::runtime_error("Expected register, got: " + s);
        int idx = std::stoi(r.substr(1));
        if (idx < 0 || idx > 31) throw std::runtime_error("Register index out of range: " + s);
        return static_cast<uint32_t>(idx);
    }

    static int32_t parseImm(const std::string& s,
                            const std::unordered_map<std::string, uint32_t>& labels) {
        if (isNumber(s)) return std::stoi(s);
        auto it = labels.find(s);
        if (it == labels.end()) throw std::runtime_error("Undefined label or literal: " + s);
        return static_cast<int32_t>(it->second);
    }

    static uint32_t resolveLabel(const std::string& token,
                                 const std::unordered_map<std::string, uint32_t>& labels) {
        if (isNumber(token)) return static_cast<uint32_t>(std::stoi(token));
        auto it = labels.find(token);
        if (it == labels.end()) throw std::runtime_error("Undefined label: " + token);
        return it->second;
    }

    static bool isNumber(const std::string& s) {
        if (s.empty()) return false;
        size_t start = (s[0] == '-' || s[0] == '+') ? 1 : 0;
        if (start == s.size()) return false;
        for (size_t i = start; i < s.size(); ++i)
            if (!std::isdigit(s[i])) return false;
        return true;
    }

    static std::vector<std::string> tokenise(const std::string& s) {
        std::vector<std::string> result;
        std::string cur;
        for (char c : s) {
            if (c == ' ' || c == '\t' || c == ',') {
                auto t = trim(cur);
                if (!t.empty()) result.push_back(t);
                cur.clear();
            } else {
                cur += c;
            }
        }
        auto t = trim(cur);
        if (!t.empty()) result.push_back(t);
        return result;
    }

    static std::string stripComment(const std::string& line) {
        size_t p = line.find('#');
        size_t q = line.find("//");
        size_t cut = std::string::npos;
        if (p != std::string::npos) cut = p;
        if (q != std::string::npos) cut = (cut == std::string::npos) ? q : std::min(cut, q);
        return (cut != std::string::npos) ? trim(line.substr(0, cut)) : line;
    }

    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r\f\v");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r\f\v");
        return str.substr(first, last - first + 1);
    }
};