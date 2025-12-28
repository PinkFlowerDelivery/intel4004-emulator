#pragma once

#include "cpu.h"
#include <cstdint>
#include <functional>
#include <unordered_map>

extern std::unordered_map<uint8_t, std::function<void(CPU4004 *, uint8_t)>>
    mapOfMnemonics;
