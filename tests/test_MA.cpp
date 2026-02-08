#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>
#include <random>
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

TEST(CountNumbers, DifferentArray) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);

    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);
    clearMemoryPastLabel(cpu, program, "nums");
    std::vector<uint16_t> values = {1, 2, 3, 2, 5, 2, 7, 8, 9, 2};
    setVariable(cpu, program, "n", values.size());
    setArray(cpu, program, "nums", values);
    setVariable(cpu, program, "target", 2);

    cpu.runProgram();
    EXPECT_EQ(cpu.ACC, 4);
}

TEST(CountNumbers, RandomArray) {
    auto program = Assembler<ACC_MA>::parseProgramLayout(codePath);

    CPU<ACC_MA> cpu;
    cpu.loadProgram(codePath);

    clearMemoryPastLabel(cpu, program, "nums");

    constexpr size_t SIZE = 50;
    constexpr uint16_t MAX_VAL = 100;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint16_t> dist(0, MAX_VAL);

    std::vector<uint16_t> values(SIZE);
    for (auto& v : values) v = dist(gen);

    std::uniform_int_distribution<size_t> idx_dist(0, SIZE - 1);
    uint16_t target = values[idx_dist(gen)];

    int expected_count = std::count(values.begin(), values.end(), target);

    setVariable(cpu, program, "n", SIZE);
    setArray(cpu, program, "nums", values);
    setVariable(cpu, program, "target", target);

    cpu.runProgram();

    EXPECT_EQ(cpu.ACC, expected_count);
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