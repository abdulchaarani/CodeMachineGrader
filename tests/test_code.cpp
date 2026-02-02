#include <gtest/gtest.h>

#include <string>

#include "Assembler.h"
#include "Cpu.h"
#include "ISA.h"
#include "test_helpers.h"

static const std::string codePath = "../code_examples/code.s";

TEST(CountNumbers, BaseCase) {
    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);
    cpu.runProgram();

    EXPECT_EQ(cpu.ACC, 3);
}

TEST(CountNumbers, DifferentTarget) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);

    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);

    setVariable(cpu, program, "target", 3);

    cpu.runProgram();
    EXPECT_EQ(cpu.ACC, 1);
}

TEST(CountNumbers, BiggerArray) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);

    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);
    auto nBefore = getVariable(cpu, program, "n");
    auto nAdded = 3;

    for (int i = 0; i < nAdded; ++i) {
        appendArrayElement(cpu, program, "nums", nBefore + i, 2);
    }

    setVariable(cpu, program, "n", nBefore + nAdded);

    cpu.runProgram();
    EXPECT_EQ(cpu.ACC, 6);
}