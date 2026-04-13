#include <gtest/gtest.h>

#include <filesystem>
#include <random>
#include <string>

#include "Assembler/Assembler.h"
#include "Cpu/Cpu.h"
#include "ISA/PolyRisc.h"
#include "test_helpers.h"

std::string codePath;

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

static void printUsage(const char* prog) {
    std::cerr << "Usage:\n"
              << "  ." << prog << " --path <path>\n\n"
              << "Example:\n"
              << "  ." << prog << " --path ../code_examples/code.s\n";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    codePath = "../code_examples/polyrisc.s";  // default

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