
#pragma once
#include <string>
#include <unordered_map>

#include "Assembler/Assembler_ACCLike.h"
#include "Cpu/Cpu_ACCLike.h"
#include "architectures.h"

template <AccLike ISA>
void setVariable(CPU_ACC<ISA>& cpu, const ProgramLayout& prog, const std::string& varName,
                 uint16_t value) {
    auto it = prog.labels.find(varName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Variable not found: " + varName);
    }
    cpu.MEM[it->second] = value;
}

template <AccLike ISA>
void setArrayElement(CPU_ACC<ISA>& cpu, const ProgramLayout& prog, const std::string& arrayName,
                     size_t index, uint16_t value) {
    auto it = prog.labels.find(arrayName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Array not found: " + arrayName);
    }
    cpu.MEM[it->second + index] = value;
}

template <AccLike ISA>
uint16_t getVariable(const CPU_ACC<ISA>& cpu, const ProgramLayout& prog,
                     const std::string& varName) {
    auto it = prog.labels.find(varName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Variable not found: " + varName);
    }
    return cpu.MEM[it->second];
}

template <AccLike ISA>
uint16_t getArrayElement(const CPU_ACC<ISA>& cpu, const ProgramLayout& prog,
                         const std::string& arrayName, size_t index) {
    auto it = prog.labels.find(arrayName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Array not found: " + arrayName);
    }
    return cpu.MEM[it->second + index];
}

template <AccLike ISA>
void appendArrayElement(CPU_ACC<ISA>& cpu, const ProgramLayout& prog, const std::string& arrayName,
                        size_t index, uint16_t value) {
    auto it = prog.labels.find(arrayName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Array not found");
    }

    cpu.MEM[it->second + index] = value;
}

template <AccLike ISA>
std::vector<uint16_t> getArray(const CPU_ACC<ISA>& cpu, const ProgramLayout& prog,
                               const std::string& arrayName, size_t arraySize) {
    auto it = prog.labels.find(arrayName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Array not found: " + arrayName);
    }

    std::vector<uint16_t> result;
    result.reserve(arraySize);
    for (size_t i = 0; i < arraySize; ++i) {
        result.push_back(cpu.MEM[it->second + i]);
    }
    return result;
}

// must set size first
template <AccLike ISA>
void setArray(CPU_ACC<ISA>& cpu, const ProgramLayout& prog, const std::string& arrayName,
              const std::vector<uint16_t>& values) {
    auto it = prog.labels.find(arrayName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Array not found: " + arrayName);
    }

    size_t startAddr = it->second;
    for (size_t i = 0; i < values.size(); ++i) {
        cpu.MEM[startAddr + i] = values[i];
    }
}

template <AccLike ISA>
void clearMemoryPastLabel(CPU_ACC<ISA>& cpu, const ProgramLayout& prog,
                          const std::string& arrayName) {
    static constexpr uint8_t memorySize = UINT8_MAX;

    auto it = prog.labels.find(arrayName);
    if (it == prog.labels.end()) {
        throw std::runtime_error("Array not found: " + arrayName);
    }

    size_t startAddr = it->second;

    for (size_t i = startAddr; i < memorySize; ++i) {
        cpu.MEM[i] = 0;
    }
}