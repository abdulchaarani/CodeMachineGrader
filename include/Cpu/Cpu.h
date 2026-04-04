// CPU.h
#pragma once
#include "Cpu_ACC.h"
#include "Cpu_Risc.h"

template <typename ISA>
struct CpuImpl;

template <AccLike ISA>
struct CpuImpl<ISA> {
    using type = CPU_ACC<ISA>;
};

template <RiscLike ISA>
struct CpuImpl<ISA> {
    using type = CPU_Risc<ISA>;
};

template <typename ISA>
using CPU = typename CpuImpl<ISA>::type;