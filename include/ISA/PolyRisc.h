#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

struct PolyRisc {
    // clang-format off
    enum class Opcode : uint32_t {
        ALU       = 0b0000,
        READ_MEM  = 0b1000,
        WRITE_MEM = 0b1001,
        LDI       = 0b1010,
        JUMP      = 0b1100,
        STOP      = 0b1111,
    };

    struct InstructionDescription {
        Opcode  format;
        uint8_t opcode;
    };

    inline static const std::unordered_map<std::string, InstructionDescription> instrMap = {
        {"add",   {Opcode::ALU,       0}},
        {"sub",   {Opcode::ALU,       1}},
        {"shr",   {Opcode::ALU,       2}},
        {"shl",   {Opcode::ALU,       3}},
        {"not",   {Opcode::ALU,       4}},
        {"and",   {Opcode::ALU,       5}},
        {"or",    {Opcode::ALU,       6}},
        {"mv",    {Opcode::ALU,       7}},
        {"ld",    {Opcode::READ_MEM,  0}},
        {"st",    {Opcode::WRITE_MEM, 0}},
        {"ldi",   {Opcode::LDI,       0}},
        {"br",    {Opcode::JUMP,      0}},
        {"brz",   {Opcode::JUMP,      1}},
        {"brnz",  {Opcode::JUMP,      2}},
        {"brlz",  {Opcode::JUMP,      3}},
        {"brgez", {Opcode::JUMP,      4}},
        {"stop",  {Opcode::STOP,      0}},
    };

    using AluOp = std::function<int16_t(int16_t, int16_t)>;

    inline static const std::unordered_map<uint8_t, AluOp> aluTable = {
        {0, [](int16_t a, int16_t b)                    { return int16_t(a + b); }},  // add
        {1, [](int16_t a, int16_t b)                    { return int16_t(a - b); }},  // sub
        {2, [](int16_t a, [[maybe_unused]] int16_t b)   { return int16_t(a >> 1); }}, // shr
        {3, [](int16_t a, [[maybe_unused]] int16_t b)   { return int16_t(a << 1); }}, // shl
        {4, [](int16_t a, [[maybe_unused]] int16_t b)   { return int16_t(~a); }},     // not
        {5, [](int16_t a, int16_t b)                    { return int16_t(a & b); }},  // and
        {6, [](int16_t a, int16_t b)                    { return int16_t(a | b); }},  // or
        {7, [](int16_t a, [[maybe_unused]] int16_t b)   { return a; }},               // mv
    };
    // clang-format on
};