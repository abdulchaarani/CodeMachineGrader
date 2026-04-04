#include <gtest/gtest.h>

#include <filesystem>
#include <random>
#include <string>

#include "Assembler/Assembler_ACCLike.h"
#include "Cpu/Cpu_ACCLike.h"
#include "ISA/ACC_MA.h"
#include "test_helpers.h"

std::string codePath;

TEST(InverserTableau, ExempleDeBasePair) {
    auto program = Assembler_ACC<ACC_MA>::parseProgramLayout(codePath);
    CPU_ACC<ACC_MA> cpu;
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
    auto program = Assembler_ACC<ACC_MA>::parseProgramLayout(codePath);
    CPU_ACC<ACC_MA> cpu;
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

    std::vector<uint16_t> randomNumbers(50);
    for (size_t i = 0; i < 50; ++i) {
        randomNumbers[i] = dist(gen);
    }

    auto program = Assembler_ACC<ACC_MA>::parseProgramLayout(codePath);
    CPU_ACC<ACC_MA> cpu;
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
    auto program = Assembler_ACC<ACC_MA>::parseProgramLayout(codePath);
    CPU_ACC<ACC_MA> cpu;
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
    auto program = Assembler_ACC<ACC_MA>::parseProgramLayout(codePath);
    CPU_ACC<ACC_MA> cpu;
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
    auto program = Assembler_ACC<ACC_MA>::parseProgramLayout(codePath);
    CPU_ACC<ACC_MA> cpu;
    cpu.loadProgram(codePath);

    std::vector<uint16_t> values = {1, 2, 3, 4, 5, 6};
    clearMemoryPastLabel(cpu.MEM, program.labels, "nums");
    setVariable(cpu.MEM, program.labels, "n", values.size());
    setArray(cpu.MEM, program.labels, "nums", values);

    cpu.runProgram();

    EXPECT_LE(cpu.nCycles, 225);
}

static void printUsage(const char* prog) {
    std::cerr << "Usage:\n"
              << "  ." << prog << " --path <path>\n\n"
              << "Example:\n"
              << "  ." << prog << " --path ../code_examples/code.s\n";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    codePath = "../code_examples/ma.s";  // default

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--path" && i + 1 < argc) {
            codePath = argv[i + 1];
            break;
        }
    }

    std::cout << codePath << '\n';
    std::cout << codePath.empty() << '\n';

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