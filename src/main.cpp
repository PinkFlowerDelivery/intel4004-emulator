#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

// INFO: https://datasheets.chipdb.org/Intel/MCS-4/datashts/intel-4004.pdf
struct CPU4004 {
  uint8_t R[15] = {0};     // each 4 bit
  uint8_t AC = 0;          // 4 bit
  uint16_t PC = 0;         // 12 bit
  uint16_t Stack[3] = {0}; // each 12 bit
  uint8_t ROM[4096] = {0};

  uint8_t fetch() {
    uint8_t opcode = ROM[PC];
    PC++;
    return opcode;
  }
};

// fetch read only 1 byte(8 bit)

int main() {
  CPU4004 cpu;

  cpu.ROM[0] = 0xD5; // LDM 5
  cpu.ROM[1] = 0xB0; // XCH
  cpu.ROM[2] = 0xD1; // LDM 1
  cpu.ROM[3] = 0x80; // ADD AC + R0
  cpu.ROM[4] = 0xD0;
  cpu.ROM[5] = 0xB0;

  cpu.PC = 0;

  while (true) {
    uint8_t opcode = cpu.fetch();

    switch (opcode >> 4) {
    case 0x8:
      std::cout << "ADD" << std::endl;
      // ADD -> AC + R0-15 == result in AC

      cpu.AC = cpu.AC + cpu.R[opcode & 0x0F];
      std::cout << cpu.AC << std::endl;
      break;

    case 0xD:
      std::cout << "LDM" << std::endl;
      cpu.AC = opcode & 0x0F;
      break;

    case 0xB:
      std::cout << "XCH" << std::endl;
      uint8_t temp = cpu.AC;
      cpu.AC = cpu.R[opcode & 0x0F];
      cpu.R[opcode & 0x0F] = temp;
    }
  }
};
