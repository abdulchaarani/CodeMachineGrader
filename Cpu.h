#pragma once

#include <cassert>
#include <cstdint>
#include <functional>
#include <unordered_map>

class Cpu {
  using Instruction = std::function<void(uint8_t)>;
  static constexpr uint8_t memorySize = UINT8_MAX;

private:
  std::unordered_map<uint8_t, Instruction> ISA = {
      {0x00, [this](uint8_t ADR) { ACC += MEM[ADR]; }},
      {0x01, [this](uint8_t ADR) { ACC -= MEM[ADR]; }},
      {0x02, [this](uint8_t ADR) { ACC *= MEM[ADR]; }},
      {0x03, [this](uint8_t ADR) { MEM[ADR] = ACC; }},
      {0x04, [this](uint8_t ADR) { ACC = MEM[ADR]; }},
      {0x05, [this](uint8_t ADR) { PC = PC; }},
      {0x07, [this](uint8_t ADR) { PC = ADR; }},                          // br
      {0x08, [this](uint8_t ADR) { ACC == 0 ? PC = ADR : PC = PC + 1; }}, // brz
      {0x08,
       [this](uint8_t ADR) { ACC != 0 ? PC = ADR : PC = PC + 1; }}, // brnz
  };

private:
  static uint16_t PC;
  static uint16_t ACC;
  static uint16_t MEM[memorySize];
};