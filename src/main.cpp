#include "cpu.h"
#include "mnemonics.h"
#include <cstdint>
#include <iostream>
#include <ostream>

// INFO: https://datasheets.chipdb.org/Intel/MCS-4/datashts/intel-4004.pdf
int main() {
  CPU4004 cpu;

  // NOP -> 0x00
  // *JUN -> 0x4_
  // DEC -> 0x6_
  // ADD -> 0x8_
  // SUB -> 0x9_
  // LD -> 0xA_
  // XCH -> 0xB_
  // LDM -> 0xD_
  // CLC -> 0xF1
  // STC -> 0xFA
  cpu.ROM = {0xFA, 0xF1, 0xFA};

  cpu.PC = 0;

  for (int i = 0; i < cpu.ROM.size(); i++) {
    uint8_t opcode = cpu.fetch();

    if (mapOfMnemonics.find(opcode >> 4) == mapOfMnemonics.end()) {
      std::cout << "Mnemonic not found." << std::endl;
      break;
    }

    mapOfMnemonics[opcode >> 4](&cpu, opcode);
    std::cout << "AC: " << static_cast<int>(cpu.AC) << std::endl;
    std::cout << "Carry: " << cpu.Carry << std::endl;
  }
};
