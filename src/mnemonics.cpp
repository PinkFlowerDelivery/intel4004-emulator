#include "cpu.h"
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <stdexcept>
#include <sys/types.h>
#include <unordered_map>

std::unordered_map<uint8_t, std::function<void(CPU4004*, uint8_t)>> mapOfMnemonics = {
    // NOP
    {0x0, [](CPU4004* cpu, uint8_t opcode) {}},
    // JCN
    {0x1,
     [](CPU4004* cpu, uint8_t opcode) {
         uint8_t addr = (cpu->ROM[cpu->PC + 1] & 0xF) >> 4;

         uint8_t C0 = (opcode & 0x8) >> 3;
         uint8_t C1 = (opcode & 0x4) >> 2;
         uint8_t C2 = (opcode & 0x2) >> 1;

         if (C0 == 0) {
             if (C1 == 1) {
                 if (cpu->AC == 0) {
                     cpu->PC = addr;
                 }
             }
             if (C2 == 1) {
                 if (cpu->Carry) {
                     cpu->PC = addr;
                 }
             }
         } else {
             if (C1 == 1) {
                 if (cpu->AC != 0) {
                     cpu->PC = addr;
                 }
             }
             if (C2 == 1) {
                 if (!cpu->Carry) {
                     cpu->PC = addr;
                 }
             }
         }

         cpu->PC += 1;
     }},
    // FIM
    {0x2,
     [](CPU4004* cpu, uint8_t opcode) {
         uint8_t reg = opcode & 0x0E;
         uint8_t data = cpu->ROM[cpu->PC + 1];

         cpu->R[reg] = data >> 4;
         cpu->R[reg + 1] = data & 0x0F;

         cpu->PC += 1;
     }},
    // FIN & JIN
    {0x3,
     [](CPU4004* cpu, uint8_t opcode) {
         bool isJin = opcode & 0x01;

         if (isJin) {
             uint8_t registerId = opcode & 0x0E;
             uint8_t regData0 = cpu->R[registerId];
             uint8_t regData1 = cpu->R[registerId + 1];

             cpu->PC = (cpu->PC & 0xF00) | ((regData0 << 4) | (regData1 & 0x0F));
         } else {
             uint8_t registerId = opcode & 0x0E;
             uint8_t regData0 = cpu->R[registerId];
             uint8_t regData1 = cpu->R[registerId + 1];

             uint8_t data = cpu->ROM[(regData0 << 4) | (regData1 & 0x0F)];

             cpu->R[registerId] = data & 0xF0;
             cpu->R[registerId + 1] = data & 0x0F;
         }
     }},
    // JUN
    {0x4,
     [](CPU4004* cpu, uint8_t opcode) {
         std::array<uint8_t, 3> nibbles;
         nibbles[0] = opcode & 0x0F;

         uint8_t nextOpcode = cpu->ROM[cpu->PC + 1];

         nibbles[1] = nextOpcode >> 4;
         nibbles[2] = nextOpcode & 0x0F;

         cpu->PC = (nibbles[0] << 8) | (nibbles[1] << 4) | nibbles[2];
     }},

    // JMS
    {0x5,
     [](CPU4004* cpu, uint8_t opcode) {
         std::array<uint8_t, 3> nibbles;

         if (cpu->Stack.size() >= 3) {
             throw std::runtime_error("Stack overflow");
         }
         cpu->Stack.push(cpu->PC);

         nibbles[0] = opcode & 0x0F;

         uint8_t nextBits = cpu->ROM[cpu->PC + 1];

         nibbles[1] = nextBits >> 4;
         nibbles[2] = nextBits & 0x0F;

         cpu->PC = (nibbles[0] << 8) | (nibbles[1] << 4) | nibbles[2];
     }},

    // INC
    {0x6, [](CPU4004* cpu, uint8_t opcode) { cpu->R[opcode & 0x0F]++; }},
    // ISZ
    {0x7,
     [](CPU4004* cpu, uint8_t opcode) {
         uint8_t reg = cpu->R[opcode & 0x0F];
         if ((reg + 1) > 15) {
             cpu->R[opcode & 0x0F] = 0;
             cpu->PC += 1;
         } else {
             cpu->PC = (cpu->PC & 0xF00) | cpu->ROM[cpu->PC + 1];
         };
     }},
    // ADD
    {0x8,
     [](CPU4004* cpu, uint8_t opcode) {
         cpu->AC += cpu->R[opcode & 0x0F] + cpu->Carry;
         cpu->Carry = false;
         if (cpu->AC & 0xF0) {
             cpu->AC &= 0xF;
             cpu->Carry = true;
         }
     }},
    // SUB
    {0x9,
     [](CPU4004* cpu, uint8_t opcode) {
         cpu->AC += ~cpu->R[opcode & 0xF] + !cpu->Carry;
         cpu->Carry = false;

         if (cpu->AC & 0xF0) {
             cpu->AC &= 0xF;
             cpu->Carry = true;
         }
     }},
    // LD
    {0xA, [](CPU4004* cpu, uint8_t opcode) { cpu->AC = cpu->R[opcode & 0x0F]; }},
    // XCH
    {0xB,
     [](CPU4004* cpu, uint8_t opcode) {
         uint8_t temp = cpu->AC;
         cpu->AC = cpu->R[opcode & 0x0F];
         cpu->R[opcode & 0x0F] = temp;
     }},
    // BBL
    {0xC,
     [](CPU4004* cpu, uint8_t opcode) {
         cpu->AC = opcode & 0x0F;
         cpu->PC = cpu->Stack.top();
         cpu->Stack.pop();
     }},
    // LDM
    {0xD, [](CPU4004* cpu, uint8_t opcode) { cpu->AC = opcode & 0x0F; }},

    // STORE
    {0xE,
     [](CPU4004* cpu, uint8_t opcode) {
         uint8_t nextByte = cpu->fetch();
         uint16_t addr = (cpu->PC & 0xF00) | nextByte;

         cpu->ROM[addr] = cpu->R[opcode & 0x0F];

         cpu->PC += 1;
     }},

    // CLC, STC
    {0xF,
     [](CPU4004* cpu, uint8_t opcode) {
         switch (opcode & 0x0F) {
         // CLB
         case 0x0: {
             cpu->Carry = false;
             cpu->AC = 0;
             break;
         }

         // CLC
         case 0x1: {
             cpu->Carry = false;
             break;
         }

         // IAC
         case 0x2: {
             cpu->AC++;

             if (cpu->AC > 15) {
                 cpu->Carry = true;
                 cpu->AC = cpu->AC & 0x0F;
             } else {
                 cpu->Carry = false;
             }
             break;
         }

         // CMC
         case 0x3: {
             cpu->Carry = !cpu->Carry;
             break;
         }

         // CMA
         case 0x4: {
             cpu->AC = ~cpu->AC;
             break;
         }

         // RAL
         case 0x5: {
             bool oldCarry = cpu->Carry;
             cpu->Carry = cpu->AC & 0x08;
             cpu->AC = ((cpu->AC << 1) | oldCarry) & 0x0F;
             break;
         }

         // RAR
         case 0x6: {
             bool oldCarry = cpu->Carry;
             cpu->Carry = cpu->AC & 0x01;
             cpu->AC = ((cpu->AC >> 1) | (oldCarry << 3)) & 0x0F;
             break;
         }

         // TCC
         case 0x7: {
             cpu->AC = 0;
             cpu->AC = cpu->Carry;
             cpu->Carry = false;
             break;
         }

         // DAC
         case 0x8: {
             if (cpu->AC == 0) {
                 cpu->AC = 15;
                 cpu->Carry = false;
             } else {
                 cpu->AC--;
                 cpu->Carry = true;
             }
             break;
         }

         // TCS
         case 0x9: {
             if (cpu->Carry) {
                 cpu->AC = 10;
             } else {
                 cpu->AC = 9;
             }
             cpu->Carry = false;
         }

         // STC
         case 0xA: {
             cpu->Carry = true;
             break;
         }

         // DAA
         case 0xB: {
             if (cpu->AC > 9 || cpu->Carry) {
                 cpu->AC += 6;
                 if (cpu->AC > 15) {
                     cpu->AC = cpu->AC & 0x0F;
                     cpu->Carry = true;
                 }
             }
         }
         }
     }},

};
