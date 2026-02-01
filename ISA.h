#pragma once

#include <cstdint>
#include <span>
#include <string>

template <typename T>
concept InstructionSet = requires {
    { std::span<const std::pair<const std::string, const uint8_t>>(T::opcodes) };
};

struct ACC {
    static constexpr uint8_t memorySize = UINT8_MAX;

    // clang-format off
    static constexpr std::pair<const std::string, const uint8_t> opcodes[] = {
        {"add", 0x00},
        {"sub", 0x01},
        {"mul", 0x02},
        {"st", 0x03},
        {"ld", 0x04},
        {"stop", 0x05},
        {"br", 0x07},
        {"brz", 0x08},
        {"brnz", 0x09},
    };

    // clang-format on
};

struct ACC_MA {
    // clang-format off
    static constexpr std::pair<const std::string, const uint8_t> opcodes[] = {
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
};