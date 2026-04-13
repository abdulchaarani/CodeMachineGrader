#include <gtest/gtest.h>

#include <cstdlib>
#include <random>
#include <string>

#include "Assembler/Assembler.h"
#include "Cpu/Cpu.h"
#include "ISA/PolyRisc.h"
#include "test_helpers.h"

const std::string codePath = [] {
    const char* env = std::getenv("CODE_PATH");
    return std::string(env ? env : CODE_PATH_DEFAULT);
}();

TEST(EncodageDiff, ExempleDeBase) {
    auto program = Assembler<PolyRisc>::parseProgramLayout(codePath);
    CPU<PolyRisc> cpu;
    cpu.loadProgram(codePath);

    setVariable(cpu.DMEM, program.dataLabels, "n", 6);
    setArray(cpu.DMEM, program.dataLabels, "table", {3, 7, 2, 9, 4, 8});
    cpu.runProgram();

    std::vector<int16_t> result = getArray(cpu.DMEM, program.dataLabels, "diff", 5);
    std::vector<int16_t> expected{4, -5, 7, -5, 4};
    EXPECT_EQ(result, expected);
}

TEST(EncodageDiff, NombreDeCycles) {
    auto program = Assembler<PolyRisc>::parseProgramLayout(codePath);
    CPU<PolyRisc> cpu;
    cpu.loadProgram(codePath);
    cpu.runProgram();

    EXPECT_LE(cpu.nCycles, 222);
}

TEST(EncodageDiff, CasAleatoireTailleFixe) {
    auto program = Assembler<PolyRisc>::parseProgramLayout(codePath);
    CPU<PolyRisc> cpu;
    cpu.loadProgram(codePath);

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int16_t> dist(-50, 50);

    constexpr int n = 6;
    std::vector<int16_t> table(n);
    for (auto& v : table) v = dist(rng);

    setVariable(cpu.DMEM, program.dataLabels, "n", n);
    setArray(cpu.DMEM, program.dataLabels, "table", table);
    cpu.runProgram();

    std::vector<int16_t> expected(n - 1);
    for (int i = 0; i < n - 1; ++i) expected[i] = table[i + 1] - table[i];

    std::vector<int16_t> result = getArray(cpu.DMEM, program.dataLabels, "diff", n - 1);
    EXPECT_EQ(result, expected);
}

TEST(EncodageDiff, CasAleatoireTailleVariable) {
    auto program = Assembler<PolyRisc>::parseProgramLayout(codePath);
    CPU<PolyRisc> cpu;
    cpu.loadProgram(codePath);

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> sizeDist(2, 10);
    std::uniform_int_distribution<int16_t> dist(-50, 50);

    const int n = sizeDist(rng);
    std::vector<int16_t> table(n);
    for (auto& v : table) v = dist(rng);

    setVariable(cpu.DMEM, program.dataLabels, "n", n);
    setArray(cpu.DMEM, program.dataLabels, "table", table);
    cpu.runProgram();

    std::vector<int16_t> expected(n - 1);
    for (int i = 0; i < n - 1; ++i) expected[i] = table[i + 1] - table[i];

    std::vector<int16_t> result = getArray(cpu.DMEM, program.dataLabels, "diff", n - 1);
    EXPECT_EQ(result, expected);
}
