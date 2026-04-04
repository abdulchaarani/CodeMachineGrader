#pragma once
#include <cstdint>

#include "ISA/ACC_MA.h"
#include "ISA/PolyRisc.h"

template <typename ISA>
struct CpuTraits;

template <>
struct CpuTraits<ACC> {
    using AddrT = uint16_t;
    using DataT = uint16_t;
    using IRT = uint16_t;

    static constexpr uint32_t MAX_CYCLES = 1024;
    static constexpr uint32_t MEM_SIZE = UINT8_MAX;
};

template <>
struct CpuTraits<ACC_MA> {
    using AddrT = uint16_t;
    using DataT = uint16_t;
    using IRT = uint16_t;

    static constexpr uint32_t MAX_CYCLES = 1024;
    static constexpr uint32_t MEM_SIZE = UINT8_MAX;
};

template <>
struct CpuTraits<PolyRisc> {
    using AddrT = uint32_t;
    using DataT = int16_t;
    using IRT = uint32_t;

    static constexpr uint32_t MAX_CYCLES = 1024;
    static constexpr uint32_t IMEM_SIZE = 4095;        // 12-bit
    static constexpr uint32_t DMEM_SIZE = UINT16_MAX;  // 16-bit
};