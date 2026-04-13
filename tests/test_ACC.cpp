#include <gtest/gtest.h>

#include <filesystem>
#include <random>
#include <string>

#include "Assembler/Assembler.h"
#include "Cpu/Cpu.h"
#include "ISA/ACC_MA.h"
#include "test_helpers.h"

std::string codePath;

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

static void printUsage(const char* prog) {
    std::cerr << "Usage:\n"
              << "  ." << prog << " --path <path>\n\n"
              << "Example:\n"
              << "  ." << prog << " --path ../code_examples/code.s\n";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    codePath = "../code_examples/acc.s";  // default

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--path" && i + 1 < argc) {
            codePath = argv[i + 1];
            break;
        }
    }

    if (codePath.empty()) {
        printUsage(argv[0]);
        return 1;
    }

    else if (!std::filesystem::exists(codePath)) {
        std::cerr << "Error: file does not exist: " << codePath << "\n";
    }

    else if (!std::filesystem::is_regular_file(codePath)) {
        std::cerr << "Error: not a regular file: " << codePath << "\n";
    }

    return RUN_ALL_TESTS();
}