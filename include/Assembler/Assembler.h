#pragma once
#include "Assembler/Assembler_ACC.h"
#include "Assembler/Assembler_Risc.h"
#include "architectures.h"

template <typename ISA>
struct AssemblerImpl;

template <AccLike ISA>
struct AssemblerImpl<ISA> {
    using type = Assembler_ACC<ISA>;
    using Layout = ProgramLayoutAcc;
};

template <RiscLike ISA>
struct AssemblerImpl<ISA> {
    using type = Assembler_Risc<ISA>;
    using Layout = ProgramLayoutRisc;
};

template <typename ISA>
using Assembler = typename AssemblerImpl<ISA>::type;

template <typename ISA>
using AssemblerLayout = typename AssemblerImpl<ISA>::Layout;