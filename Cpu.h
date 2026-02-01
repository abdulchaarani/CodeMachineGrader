#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <unordered_map>

#include "Assembler.h"
#include "ISA.h"

template <InstructionSet ISA>
class CPU {
    using Instruction = std::function<void(uint8_t)>;
    static constexpr uint8_t memorySize = UINT8_MAX;

   public:
    void loadProgram(const std::string& filename) {
        const auto program = Assembler<ISA>::assemble(filename);
        for (uint8_t i = 0; i < program.size(); ++i) {
            MEM[i] = program[i];
        }
    }

    void runProgram() {
        isCpuRunning = true;
        while (isCpuRunning) {
            fetch();
            decode();
            execute();
        }
        std::cout << ACC << '\n';
    }

    void fetch() { IR = MEM[PC++]; };
    void decode() {
        currOpcode = IR >> 8;
        currData = IR;
    }
    void execute() {
        static auto& table = dispatch();
        table[currOpcode](currData);
    }

   private:
    bool isCpuRunning = false;

    uint16_t PC = 0;
    uint16_t IR = 0;
    uint16_t ACC = 0;
    uint16_t MA = 0;
    std::array<uint16_t, memorySize> MEM{};

    uint8_t currOpcode = 0;
    uint8_t currData = 0;

    std::unordered_map<uint8_t, Instruction>& dispatch();
};

// clang-format off
template <>
inline std::unordered_map<uint8_t, CPU<ACC>::Instruction>& CPU<ACC>::dispatch() {
    static std::unordered_map<uint8_t, Instruction> table = {
        {0x00, [this](uint8_t ADR) { ACC += MEM[ADR]; }},           // add
        {0x01, [this](uint8_t ADR) { ACC -= MEM[ADR]; }},           // sub
        {0x02, [this](uint8_t ADR) { ACC *= MEM[ADR]; }},           // mul
        {0x03, [this](uint8_t ADR) { MEM[ADR] = ACC; }},            // st
        {0x04, [this](uint8_t ADR) { ACC = MEM[ADR]; }},            // ld
        {0x05, [this](uint8_t) { isCpuRunning = false; }},          // stop
        {0x07, [this](uint8_t ADR) { PC = ADR; }},                  // br
        {0x08, [this](uint8_t ADR) { if (ACC == 0) PC = ADR; }},    // brz
        {0x09, [this](uint8_t ADR) { if (ACC != 0) PC = ADR; }},    // brnz
    };
    return table;
}
// clang-format on

// clang-format off
template <>
inline std::unordered_map<uint8_t, CPU<ACC_MA>::Instruction>& CPU<ACC_MA>::dispatch() {
    static std::unordered_map<uint8_t, Instruction> table = {
        {0x00, [this](uint8_t ADR) { ACC += MEM[ADR]; }},           // add
        {0x01, [this](uint8_t ADR) { ACC -= MEM[ADR]; }},           // sub
        {0x02, [this](uint8_t ADR) { ACC *= MEM[ADR]; }},           // mul
        {0x03, [this](uint8_t ADR) { MA += MEM[ADR]; }},            // adda
        {0x04, [this](uint8_t ADR) { MA -= MEM[ADR]; }},            // suba
        {0x05, [this](uint8_t) { ACC += MEM[MA]; }},                // addx
        {0x06, [this](uint8_t) { ACC -= MEM[MA]; }},                // subx
        {0x07, [this](uint8_t ADR) { ACC = MEM[ADR]; }},            // ld
        {0x08, [this](uint8_t ADR) { MEM[ADR] = ACC; }},            // st
        {0x09, [this](uint8_t ADR) { MA = MEM[ADR]; }},             // lda
        {0x0A, [this](uint8_t ADR) { MEM[ADR] = MA; }},            // sta
        {0x0B, [this](uint8_t) {ACC = MEM[MA]; }},                 // ldi
        {0x0C, [this](uint8_t) { MEM[MA] = ACC; }},                // sti
        {0x0D, [this](uint8_t ADR) { PC = ADR; }},                 // br
        {0x0E, [this](uint8_t ADR) { if (ACC == 0) PC = ADR; }},   // brz
        {0x0F, [this](uint8_t ADR) { if (ACC != 0) PC = ADR; }},   // brnz
        {0x10, [this](uint8_t) { ACC <<= 1; }},                    // shl
        {0x11, [this](uint8_t) { ACC >>= 1; }},                    // shr
        {0x13, [this](uint8_t) { isCpuRunning = false;}},          // stop
    };
    return table;
}
// clang-format on
