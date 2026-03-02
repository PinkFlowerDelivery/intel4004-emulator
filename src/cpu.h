#pragma once
#include <array>
#include <cstdint>
#include <stack>

struct CPU4004 {
    std::array<uint8_t, 16> R = {}; // each 4 bit
    uint8_t AC = 0;                 // 4 bit
    uint16_t PC = 0;                // 12 bit
    std::stack<uint16_t> Stack;     // each 12 bit
    std::array<uint8_t, 4096> ROM = {};
    bool Carry = false;

    uint8_t fetch();
};
