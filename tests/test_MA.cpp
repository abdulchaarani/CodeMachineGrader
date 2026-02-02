#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "Assembler.h"
#include "Cpu.h"
#include "ISA.h"
#include "test_helpers.h"

std::string codePath;

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

static void printUsage(const char* prog) {
    std::cerr << "Usage:\n"
              << "  " << prog << " --path <path>\n\n"
              << "Example:\n"
              << "  " << prog << " --path ../code_examples/code.s\n";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--path" && i + 1 < argc) {
            codePath = argv[i + 1];
            break;
        }
    }

    if (codePath.empty()) {
        printUsage(argv[0]);
    }

    else if (!std::filesystem::exists(codePath)) {
        std::cerr << "Error: file does not exist: " << codePath << "\n";
    }

    else if (!std::filesystem::is_regular_file(codePath)) {
        std::cerr << "Error: not a regular file: " << codePath << "\n";
    }

    return RUN_ALL_TESTS();
}