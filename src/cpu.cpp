#include "cpu.h"
#include <cstdint>

uint8_t CPU4004::fetch() {
  uint8_t opcode = ROM[PC];
  PC++;
  return opcode;
}
