#pragma once
#include <concepts>

template <typename T>
concept AccLike = requires {
    { T::opcodes };
};

template <typename T>
concept RiscLike = requires {
    typename T::Opcode;
    typename T::InstructionDescription;
    requires std::is_enum_v<typename T::Opcode>;
    { T::instrMap };
    { T::aluTable };
};