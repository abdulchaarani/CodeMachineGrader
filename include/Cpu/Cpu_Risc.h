#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "Assembler/Assembler.h"
#include "CpuTraits.h"
#include "architectures.h"

template <RiscLike ISA>
class CPU_Risc {
    using Traits = CpuTraits<ISA>;
    using AddrT = typename Traits::AddrT;
    using DataT = typename Traits::DataT;
    using IRT = typename Traits::IRT;

   public:
    void loadProgram(const std::string& filename) {
        if (!std::filesystem::exists(filename))
            throw std::runtime_error("File does not exist: " + filename);
        loadLayout(Assembler<ISA>::parseProgramLayout(filename));
    }

    void loadLayout(const ProgramLayoutRisc& layout) {
        auto words = Assembler_Risc<ISA>::assemble(layout);

        uint32_t iAddr = 0;
        for (size_t i = 0; i < layout.text.size(); ++i) {
            if (iAddr >= Traits::IMEM_SIZE) throw std::runtime_error("Instruction memory overflow");
            IMEM[iAddr++] = words[i];
        }

        uint32_t dAddr = 0;
        for (size_t i = layout.text.size(); i < words.size(); ++i) {
            if (dAddr >= Traits::DMEM_SIZE) throw std::runtime_error("Data memory overflow");
            DMEM[dAddr++] = static_cast<DataT>(words[i]);
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
        if (PC >= Traits::IMEM_SIZE) throw std::runtime_error("Program counter out of bounds");
        if (nCycles >= Traits::MAX_CYCLES)
            throw std::runtime_error("Exceeded maximum number of cycles");
        IR = IMEM[PC];
        ++nCycles;
    }

    // clang-format off
    void decode() {
        bits27_24 = (IR >> 24) & 0xF;
        bit27     = (IR >> 27) & 0x1;
        op_alu    = (IR >> 24) & 0x7;
        rdst      = (IR >> 16) & 0x1F;
        rsrc1     = (IR >>  8) & 0x1F;
        rsrc2     = (IR >>  0) & 0x1F;
        disp16    = (IR >>  0) & 0xFFFF;
        disp12    = (IR >>  0) & 0xFFF;
        jtype     = (IR >> 16) & 0xF;
        ++nCycles;
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
        ++nCycles;
    }

   public:
    AddrT PC = 0;
    IRT IR = 0;

    std::array<DataT, 32> REG{};
    std::array<AddrT, Traits::IMEM_SIZE> IMEM{};
    std::array<DataT, Traits::DMEM_SIZE> DMEM{};

    bool flagZ = false;
    bool flagN = false;
    uint32_t nCycles = 0;

   private:
    void executeALU() {
        DataT a = REG[rsrc1];
        DataT b = REG[rsrc2];
        auto it = ISA::aluTable.find(op_alu);
        if (it == ISA::aluTable.end())
            throw std::runtime_error("Unknown ALU op: " + std::to_string(op_alu));
        DataT result = it->second(a, b);
        REG[rdst] = result;
        updateFlags(result);
        ++PC;
    }

    void executeLd() {
        uint32_t addr = static_cast<uint32_t>(REG[rsrc1]);
        if (addr >= Traits::DMEM_SIZE)
            throw std::runtime_error("Data memory read out of bounds: " + std::to_string(addr));
        REG[rdst] = DMEM[addr];
        ++PC;
    }

    void executeSt() {
        uint32_t addr = static_cast<uint32_t>(REG[rsrc1]);
        if (addr >= Traits::DMEM_SIZE)
            throw std::runtime_error("Data memory write out of bounds: " + std::to_string(addr));
        DMEM[addr] = REG[rsrc2];
        ++PC;
    }

    void executeLdi() {
        REG[rdst] = static_cast<DataT>(disp16);
        ++PC;
    }

    void executeJump() {
        bool taken = false;
        switch (jtype) {
            case 0:
                taken = true;
                break;  // br
            case 1:
                taken = flagZ;
                break;  // brz
            case 2:
                taken = !flagZ;
                break;  // brnz
            case 3:
                taken = flagN;
                break;  // brlz
            case 4:
                taken = !flagN;
                break;  // brgez
            default:
                throw std::runtime_error("Unknown jtype: " + std::to_string(jtype));
        }
        if (taken) {
            if (disp12 >= Traits::IMEM_SIZE)
                throw std::runtime_error("Branch target out of bounds: " + std::to_string(disp12));
            PC = disp12;
        } else {
            ++PC;
        }
    }

    void updateFlags(DataT result) {
        flagZ = (result == 0);
        flagN = (result < 0);
    }

   private:
    bool isRunning = false;

    AddrT bits27_24 = 0;
    AddrT bit27 = 0;
    AddrT op_alu = 0;
    AddrT rdst = 0;
    AddrT rsrc1 = 0;
    AddrT rsrc2 = 0;
    AddrT disp16 = 0;
    AddrT disp12 = 0;
    AddrT jtype = 0;
};