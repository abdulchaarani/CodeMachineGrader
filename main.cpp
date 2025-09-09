#include <string>
#include <unordered_map>
#include <vector>

#include "Cpu.h"
#include "ISA.h"

std::vector<std::string> split(const std::string& s, char delimiter = ' ') {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

// clang-format off
std::unordered_map<std::string, uint8_t> opcodeMap = {
    {"add", 0x00},
    {"sub", 0x01},
    {"mul", 0x02},
    {"adda", 0x03},
    {"suba", 0x04},
    {"addx", 0x05},
    {"subx", 0x06},
    {"ld", 0x07},
    {"st", 0x08},
    {"lda", 0x09},
    {"sta", 0x0A},
    {"ldi", 0x0B},
    {"sti", 0x0C},
    {"br", 0x0D},
    {"brz", 0x0E},
    {"brnz", 0x0F},
    {"shl", 0x10},
    {"shr", 0x11},
    {"stop", 0x13},
};

// clang-format on

int main() {
    CPU<ACC_MA> cpu;
    cpu.loadProgram("code.s");
    cpu.runProgram();
}