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

TEST(Fibonacci, Exemple4) {
    auto program = Assembler<ACC>::parseProgramLayout(codePath);
    CPU<ACC> cpu;
    cpu.loadProgram(codePath);

    setVariable(cpu.MEM, program.labels, "n", 4);

    cpu.runProgram();

    EXPECT_EQ(cpu.ACC, 3);
}

TEST(Fibonacci, Exemple10) {
    auto program = Assembler<ACC>::parseProgramLayout(codePath);
    CPU<ACC> cpu;
    cpu.loadProgram(codePath);

    setVariable(cpu.MEM, program.labels, "n", 10);

    cpu.runProgram();

    EXPECT_EQ(cpu.ACC, 55);
}

TEST(Fibonacci, Aleatoire) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(2, 18);
    int n = dist(rng);

    auto fib = [](int k) {
        int a = 0, b = 1;
        for (int i = 0; i < k - 1; ++i) {
            int t = a + b;
            a = b;
            b = t;
        }
        return b;
    };

    auto program = Assembler<ACC>::parseProgramLayout(codePath);
    CPU<ACC> cpu;
    cpu.loadProgram(codePath);

    setVariable(cpu.MEM, program.labels, "n", n);

    cpu.runProgram();

    EXPECT_EQ(cpu.ACC, fib(n)) << "n = " << n;
}

TEST(Fibonacci, NombreDeCycles) {
    auto program = Assembler<ACC>::parseProgramLayout(codePath);
    CPU<ACC> cpu;
    cpu.loadProgram(codePath);

    setVariable(cpu.MEM, program.labels, "n", 10);

    cpu.runProgram();

    EXPECT_LE(cpu.nCycles, 750);
}

