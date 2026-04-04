#include <gtest/gtest.h>

#include <filesystem>
#include <random>
#include <string>

#include "Assembler/Assembler.h"
#include "Cpu/Cpu.h"
#include "ISA/PolyRisc.h"
#include "test_helpers.h"

std::string codePath;

TEST(DecToHex, ExempleDeBase) {
    auto program = Assembler<PolyRisc>::parseProgramLayout(codePath);
    CPU<PolyRisc> cpu;
    cpu.loadProgram(codePath);

    setVariable(cpu.DMEM, program.dataLabels, "valeur", 6699);

    cpu.runProgram();

    std::vector<int16_t> result = getArray(cpu.DMEM, program.dataLabels, "hex", 4);
    std::vector<int16_t> expected{11, 2, 10, 1};
    EXPECT_EQ(result, expected);
}

TEST(DecToHex, CasAleatoire) {
    srand(static_cast<unsigned>(time(nullptr)));
    uint16_t input = rand() % 65536;

    std::vector<int16_t> expected = {
        static_cast<int16_t>((input >> 0) & 0xF),
        static_cast<int16_t>((input >> 4) & 0xF),
        static_cast<int16_t>((input >> 8) & 0xF),
        static_cast<int16_t>((input >> 12) & 0xF),
    };

    auto program = Assembler<PolyRisc>::parseProgramLayout(codePath);
    CPU<PolyRisc> cpu;
    cpu.loadProgram(codePath);
    setVariable(cpu.DMEM, program.dataLabels, "valeur", static_cast<int16_t>(input));
    cpu.runProgram();

    std::vector<int16_t> result = getArray(cpu.DMEM, program.dataLabels, "hex", 4);
    EXPECT_EQ(result, expected);
}

TEST(DecToHex, NombreDeCycles) {
    auto program = Assembler<PolyRisc>::parseProgramLayout(codePath);
    CPU<PolyRisc> cpu;
    cpu.loadProgram(codePath);
    cpu.runProgram();

    EXPECT_LE(cpu.nCycles, 156);
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