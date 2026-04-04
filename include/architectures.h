#pragma once
#include <concepts>
#include <cstdint>
#include <span>
#include <string>

template <typename T>
concept AccLike = requires {
    { std::span<const std::pair<const std::string, const uint8_t>>(T::opcodes) };
    { T::memorySize } -> std::same_as<const uint8_t&>;
};

template <typename T>
concept RiscLike = requires {
    typename T::Opcode;
    typename T::InstructionDescription;
    requires std::is_enum_v<typename T::Opcode>;
    { T::instrMap };
    { T::aluTable };
};