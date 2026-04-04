#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "Assembler/Assembler_RiskLike.h"
#include "architectures.h"

template <RiscLike ISA>
class CPU_Risc {
    static constexpr uint16_t MAX_CYCLES = 1024;

   public:
    void loadProgram(const std::string& filename) {
        if (!std::filesystem::exists(filename))
            throw std::runtime_error("File does not exist: " + filename);

        auto layout = Assembler_Risc<ISA>::parseProgramLayout(filename);
        loadLayout(layout);
    }

    void loadLayout(const ProgramLayoutRisc& layout) {
        auto words = Assembler_Risc<ISA>::assemble(layout);

        uint32_t iAddr = 0;
        for (size_t i = 0; i < layout.text.size(); ++i) {
            if (iAddr >= IMEM_SIZE) throw std::runtime_error("Instruction memory overflow");
            IMEM[iAddr++] = words[i];
        }

        uint32_t dAddr = 0;
        for (size_t i = layout.text.size(); i < words.size(); ++i) {
            if (dAddr >= DMEM_SIZE) throw std::runtime_error("Data memory overflow");
            DMEM[dAddr++] = static_cast<int16_t>(words[i]);
        }
    }

    void runProgram() {
        isRunning = true;
        while (isRunning) {
            fetch();
            decode();
            execute();
        }
    }

   private:
    void fetch() {
        if (PC >= IMEM_SIZE) throw std::runtime_error("Program counter out of bounds");

        if (nCycles >= MAX_CYCLES) {
            throw std::runtime_error("Exceeded maximum number of cycles");
        }

        IR = IMEM[PC++];
        nCycles++;
    }

    // clang-format off
    void decode() {
        bits27_24   = (IR >> 24) & 0xF;
        bit27       = (IR >> 27) & 0x1;
        op_alu      = (IR >> 24) & 0x7;     // bits 26-24
        rdst        = (IR >> 16) & 0x1F;    // bits 20-16
        rsrc1       = (IR >> 8)  & 0x1F;    // bits 12-8
        rsrc2       = (IR >> 0)  & 0x1F;    // bits 5-0
        disp16      = (IR >> 0)  & 0xFFFF;  // bits 16-0
        disp12      = (IR >> 0)  & 0xFFF;   // bits 12-0
        jtype       = (IR >> 16) & 0xF;     // bits 19-16
        nCycles++;
    }
    // clang-format on

    void execute() {
        if (bit27 == 0) {
            executeALU();
        } else {
            switch (bits27_24) {
                case 0x8:
                    executeLd();
                    break;
                case 0x9:
                    executeSt();
                    break;
                case 0xA:
                    executeLdi();
                    break;
                case 0xC:
                    executeJump();
                    break;
                case 0xF:
                    isRunning = false;
                    break;
                default:
                    throw std::runtime_error("Unknown opcode nibble: " + std::to_string(bits27_24));
            }
        }
        nCycles++;
    }

    void executeALU() {
        int16_t a = REG[rsrc1];
        int16_t b = REG[rsrc2];
        auto it = ISA::aluTable.find(op_alu);
        if (it == ISA::aluTable.end())
            throw std::runtime_error("Unknown ALU op: " + std::to_string(op_alu));
        int16_t result = it->second(a, b);
        REG[rdst] = result;
        updateFlags(result);
    }

    void executeLd() {
        uint16_t addr = static_cast<uint16_t>(REG[rsrc1]);
        if (addr >= DMEM_SIZE)
            throw std::runtime_error("Data memory read out of bounds: " + std::to_string(addr));
        REG[rdst] = DMEM[addr];
    }

    void executeSt() {
        uint16_t addr = static_cast<uint16_t>(REG[rsrc1]);
        if (addr >= DMEM_SIZE)
            throw std::runtime_error("Data memory write out of bounds: " + std::to_string(addr));
        DMEM[addr] = REG[rsrc2];
    }

    void executeLdi() { REG[rdst] = static_cast<uint16_t>(disp16); }

    void executeJump() {
        bool taken = false;
        switch (jtype) {
            case 0:
                taken = true;
                break;  // br
            case 1:
                taken = flagZ;
                break;  // brz  — Z == 1
            case 2:
                taken = !flagZ;
                break;  // brnz — Z == 0
            case 3:
                taken = flagN;
                break;  // brlz — N == 1
            case 4:
                taken = !flagN;
                break;  // brgez— N == 0
            default:
                throw std::runtime_error("Unknown jtype: " + std::to_string(jtype));
        }
        if (taken) {
            if (disp12 >= IMEM_SIZE)
                throw std::runtime_error("Branch target out of bounds: " + std::to_string(disp12));
            PC = disp12;
        }
    }

    void updateFlags(int16_t result) {
        flagZ = (result == 0);
        flagN = (result < 0);
    }

   public:
    static constexpr uint32_t IMEM_SIZE = 4095;        // 12 bits
    static constexpr uint32_t DMEM_SIZE = UINT16_MAX;  // 16 bits

    uint32_t PC = 0;
    uint32_t IR = 0;

    std::array<int16_t, 32> REG{};
    std::array<uint32_t, IMEM_SIZE> IMEM{};
    std::array<int16_t, DMEM_SIZE> DMEM{};

    bool flagZ = false;
    bool flagN = false;

    uint64_t nCycles = 0;

   private:
    bool isRunning = false;

    uint32_t bits27_24 = 0;
    uint32_t bit27 = 0;
    uint32_t op_alu = 0;
    uint32_t rdst = 0;
    uint32_t rsrc1 = 0;
    uint32_t rsrc2 = 0;
    uint32_t disp16 = 0;
    uint32_t disp12 = 0;
    uint32_t jtype = 0;
};