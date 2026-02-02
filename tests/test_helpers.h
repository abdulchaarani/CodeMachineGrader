
#pragma once
#include <string>
#include <unordered_map>

#include "Assembler.h"
#include "Cpu.h"
#include "ISA.h"

template <InstructionSet ISA>
void setVariable(CPU<ISA>& cpu, const ProgramLayout& prog, const std::string& varName,
                 uint16_t value) {
    auto it = prog.labels.find(varName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Variable not found: " + varName);
    }
    cpu.MEM[it->second] = value;
}

template <InstructionSet ISA>
void setArrayElement(CPU<ISA>& cpu, const ProgramLayout& prog, const std::string& arrayName,
                     size_t index, uint16_t value) {
    auto it = prog.labels.find(arrayName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Array not found: " + arrayName);
    }
    cpu.MEM[it->second + index] = value;
}

template <InstructionSet ISA>
uint16_t getVariable(const CPU<ISA>& cpu, const ProgramLayout& prog, const std::string& varName) {
    auto it = prog.labels.find(varName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Variable not found: " + varName);
    }
    return cpu.MEM[it->second];
}

template <InstructionSet ISA>
uint16_t getArrayElement(const CPU<ISA>& cpu, const ProgramLayout& prog,
                         const std::string& arrayName, size_t index) {
    auto it = prog.labels.find(arrayName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Array not found: " + arrayName);
    }
    return cpu.MEM[it->second + index];
}

template <InstructionSet ISA>
void appendArrayElement(CPU<ISA>& cpu, const ProgramLayout& prog, const std::string& arrayName,
                        size_t index, uint16_t value) {
    auto it = prog.labels.find(arrayName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Array not found");
    }

    cpu.MEM[it->second + index] = value;
}
