#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Assembler/Assembler.h"
#include "CpuTraits.h"
#include "ISA/ACC_MA.h"
#include "architectures.h"

template <AccLike ISA>
class CPU_ACC {
    using Traits = CpuTraits<ISA>;
    using AddrT = typename Traits::AddrT;
    using DataT = typename Traits::DataT;
    using IRT = typename Traits::IRT;

    using Instruction = std::function<void(uint8_t)>;

   public:
    CPU_ACC() { initializeDispatchTable(); }

    void loadProgram(const std::string& filename) {
        if (!std::filesystem::exists(filename))
            throw std::runtime_error("File does not exist: " + filename);
        const auto program = Assembler<ISA>::assemble(filename);
        for (AddrT i = 0; i < program.size(); ++i) MEM[i] = program[i];
    }

    void loadProgramLayout(const ProgramLayoutAcc& prog) {
        const auto program = Assembler<ISA>::assemble(prog);
        for (AddrT i = 0; i < program.size(); ++i) MEM[i] = program[i];
    }

    void runProgram() {
        isRunning = true;
        while (isRunning) {
            fetch();
            decode();
            execute();
        }
    }

    void fetch() {
        if (PC >= Traits::MEM_SIZE) throw std::runtime_error("Program counter out of bounds");
        if (nCycles >= Traits::MAX_CYCLES)
            throw std::runtime_error("Exceeded maximum number of cycles");
        IR = MEM[PC++];
        ++nCycles;
    }

    void decode() {
        currOpcode = (IR >> 8) & 0xFF;
        currData = IR & 0xFF;
        ++nCycles;
    }

    void execute() {
        auto it = dispatchTable.find(currOpcode);
        if (it == dispatchTable.end()) throw std::runtime_error("Invalid opcode");
        it->second(currData);
        ++nCycles;
    }

   public:
    AddrT PC = 0;
    IRT IR = 0;
    DataT ACC = 0;
    DataT MA = 0;
    std::array<DataT, Traits::MEM_SIZE> MEM{};

    uint8_t currOpcode = 0;
    uint8_t currData = 0;
    uint32_t nCycles = 0;

   private:
    bool isRunning = false;
    std::unordered_map<uint8_t, Instruction> dispatchTable;

    void initializeDispatchTable();
};

// clang-format off
template <>
inline void CPU_ACC<ACC>::initializeDispatchTable() {
    dispatchTable = {
        {0x00, [this](uint8_t ADR) { ACC += MEM[ADR]; }},          // add
        {0x01, [this](uint8_t ADR) { ACC -= MEM[ADR]; }},          // sub
        {0x02, [this](uint8_t ADR) { ACC *= MEM[ADR]; }},          // mul
        {0x03, [this](uint8_t ADR) { MEM[ADR] = ACC; }},           // st
        {0x04, [this](uint8_t ADR) { ACC = MEM[ADR]; }},           // ld
        {0x05, [this](uint8_t)     { isRunning = false; }},         // stop
        {0x07, [this](uint8_t ADR) { PC = ADR; }},                  // br
        {0x08, [this](uint8_t ADR) { if (ACC == 0) PC = ADR; }},    // brz
        {0x09, [this](uint8_t ADR) { if (ACC != 0) PC = ADR; }},    // brnz
    };
}

template <>
inline void CPU_ACC<ACC_MA>::initializeDispatchTable() {
    dispatchTable = {
        {0x00, [this](uint8_t ADR) { ACC += MEM[ADR]; }},          // add
        {0x01, [this](uint8_t ADR) { ACC -= MEM[ADR]; }},          // sub
        {0x02, [this](uint8_t ADR) { ACC *= MEM[ADR]; }},          // mul
        {0x03, [this](uint8_t ADR) { MA  += MEM[ADR]; }},          // adda
        {0x04, [this](uint8_t ADR) { MA  -= MEM[ADR]; }},          // suba
        {0x05, [this](uint8_t)     { ACC += MEM[MA]; }},           // addx
        {0x06, [this](uint8_t)     { ACC -= MEM[MA]; }},           // subx
        {0x07, [this](uint8_t ADR) { ACC  = MEM[ADR]; }},          // ld
        {0x08, [this](uint8_t ADR) { MEM[ADR] = ACC; }},           // st
        {0x09, [this](uint8_t ADR) { MA   = MEM[ADR]; }},          // lda
        {0x0A, [this](uint8_t ADR) { MEM[ADR] = MA; }},            // sta
        {0x0B, [this](uint8_t)     { ACC  = MEM[MA]; }},           // ldi
        {0x0C, [this](uint8_t)     { MEM[MA]  = ACC; }},           // sti
        {0x0D, [this](uint8_t ADR) { PC = ADR; }},                  // br
        {0x0E, [this](uint8_t ADR) { if (ACC == 0)  PC = ADR; }},   // brz
        {0x0F, [this](uint8_t ADR) { if (ACC != 0)  PC = ADR; }},   // brnz
        {0x10, [this](uint8_t)     { ACC <<= 1; }},                 // shl
        {0x11, [this](uint8_t)     { ACC >>= 1; }},                 // shr
        {0x12, [this](uint8_t ADR) { MA = ADR; }},                  // lea
        {0x13, [this](uint8_t)     { isRunning = false; }},         // stop
    };
}
// clang-format on