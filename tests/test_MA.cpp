#include <gtest/gtest.h>

#include <cstdlib>
#include <random>
#include <string>

#include "Assembler/Assembler.h"
#include "Cpu/Cpu.h"
#include "ISA/ACC_MA.h"
#include "test_helpers.h"

const std::string codePath = [] {
    const char* env = std::getenv("CODE_PATH");
    return std::string(env ? env : CODE_PATH_DEFAULT);
}();

TEST(InverserTableau, ExempleDeBasePair) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);
    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);

    std::vector<uint16_t> values = {1, 2, 3, 4, 5, 6};
    clearMemoryPastLabel(cpu.MEM, program.labels, "nums");
    setVariable(cpu.MEM, program.labels, "n", values.size());
    setArray(cpu.MEM, program.labels, "nums", values);

    cpu.runProgram();

    std::vector<uint16_t> result = getArray(cpu.MEM, program.labels, "nums", values.size());
    std::reverse(values.begin(), values.end());

    EXPECT_EQ(values, result);
}

TEST(InverserTableau, TailleImpair) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);
    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);

    std::vector<uint16_t> values = {1, 2, 3, 4, 5, 6, 7};
    clearMemoryPastLabel(cpu.MEM, program.labels, "nums");
    setVariable(cpu.MEM, program.labels, "n", values.size());
    setArray(cpu.MEM, program.labels, "nums", values);

    cpu.runProgram();

    std::vector<uint16_t> result = getArray(cpu.MEM, program.labels, "nums", values.size());
    std::reverse(values.begin(), values.end());

    EXPECT_EQ(values, result);
}

TEST(InverserTableau, TableauLargeAleatoire) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 500);
    const int amount = 25;

    std::vector<uint16_t> randomNumbers(amount);
    for (size_t i = 0; i < amount; ++i) {
        randomNumbers[i] = dist(gen);
    }

    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);
    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);

    clearMemoryPastLabel(cpu.MEM, program.labels, "nums");
    setVariable(cpu.MEM, program.labels, "n", randomNumbers.size());
    setArray(cpu.MEM, program.labels, "nums", randomNumbers);

    cpu.runProgram();

    std::vector<uint16_t> result = getArray(cpu.MEM, program.labels, "nums", randomNumbers.size());
    std::reverse(randomNumbers.begin(), randomNumbers.end());

    EXPECT_EQ(randomNumbers, result);
}

TEST(InverserTableau, MemeNombres) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);
    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);

    std::vector<uint16_t> values = {4, 4, 4, 4};
    clearMemoryPastLabel(cpu.MEM, program.labels, "nums");
    setVariable(cpu.MEM, program.labels, "n", values.size());
    setArray(cpu.MEM, program.labels, "nums", values);

    cpu.runProgram();

    std::vector<uint16_t> result = getArray(cpu.MEM, program.labels, "nums", values.size());
    std::reverse(values.begin(), values.end());

    EXPECT_EQ(values, result);
}

TEST(InverserTableau, AvecZeros) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);
    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);

    std::vector<uint16_t> values = {0, 1, 0, 2};
    clearMemoryPastLabel(cpu.MEM, program.labels, "nums");
    setVariable(cpu.MEM, program.labels, "n", values.size());
    setArray(cpu.MEM, program.labels, "nums", values);

    cpu.runProgram();

    std::vector<uint16_t> result = getArray(cpu.MEM, program.labels, "nums", values.size());
    std::reverse(values.begin(), values.end());

    EXPECT_EQ(values, result);
}

TEST(InverserTableau, NombreDeCycles) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);
    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);

    std::vector<uint16_t> values = {1, 2, 3, 4, 5, 6};
    clearMemoryPastLabel(cpu.MEM, program.labels, "nums");
    setVariable(cpu.MEM, program.labels, "n", values.size());
    setArray(cpu.MEM, program.labels, "nums", values);

    cpu.runProgram();

    EXPECT_LE(cpu.nCycles, 225);
}

