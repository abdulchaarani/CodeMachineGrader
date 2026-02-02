#include <algorithm>
#include <cctype>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include "Cpu.h"
#include "ISA.h"

std::string toLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return str;
}

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " <cpu_type> <filepath>\n";
    std::cerr << "  cpu_type: acc or ma (case-insensitive)\n";
    std::cerr << "  filepath: path to assembly file (.s)\n";
    std::cerr << "\nExample:\n";
    std::cerr << "  " << programName << " acc code.s\n";
    std::cerr << "  " << programName << " ma code.s\n";
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            printUsage(argv[0]);
            return 1;
        }

        std::string cpuType = toLower(argv[1]);
        const std::string codePath{argv[2]};

        if (!std::filesystem::exists(codePath)) {
            std::cerr << "Error: File '" << codePath << "' does not exist\n";
            return 1;
        }

        if (!std::filesystem::is_regular_file(codePath)) {
            std::cerr << "Error: '" << codePath << "' is not a regular file\n";
            return 1;
        }

        if (cpuType == "acc") {
            CPU<ACC> cpu;
            cpu.loadProgram(codePath);
            cpu.runProgram();
            std::cout << cpu.ACC << '\n';
        } else if (cpuType == "ma") {
            CPU<ACC_MA> cpu;
            cpu.loadProgram(codePath);
            cpu.runProgram();
            std::cout << cpu.ACC << '\n';
        } else {
            std::cerr << "Error: Invalid CPU type '" << argv[1] << "'\n";
            std::cerr << "Valid types: acc, ma\n";
            printUsage(argv[0]);
            return 1;
        }

    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
        return 1;
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << '\n';
        return 1;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Error: Please verify that the code first compiles in CodeMachine\n";
        return 1;
    }

    return 0;
}