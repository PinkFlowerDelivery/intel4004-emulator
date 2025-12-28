#pragma once
#include <array>
#include <cstdint>

struct CPU4004 {
  std::array<uint8_t, 16> R = {0};     // each 4 bit
  uint8_t AC = 0;                      // 4 bit
  uint16_t PC = 0;                     // 12 bit
  std::array<uint16_t, 3> Stack = {0}; // each 12 bit
  std::array<uint8_t, 4096> ROM = {0};
  bool Carry = false;

  uint8_t fetch();
};
