#include "cpu.h"
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <unordered_map>

std::unordered_map<uint8_t, std::function<void(CPU4004*, uint8_t)>> mapOfMnemonics = {
    // NOP
    {0x0, [](CPU4004* cpu, uint8_t opcode) { cpu->PC++; }},
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

         cpu->PC += 2;
     }},
    // FIM
    {0x2,
     [](CPU4004* cpu, uint8_t opcode) {
         uint8_t reg = opcode & 0x0E;
         uint8_t data = cpu->ROM[cpu->PC + 1];

         cpu->R[reg] = data >> 4;
         cpu->R[reg + 1] = data & 0x0F;

         cpu->PC += 2;
     }},
    // FIN & JIN
    {0x3,
     [](CPU4004* cpu, uint8_t opcode) {
         bool isJin = opcode & 0x01;

         if (isJin) {
             uint8_t registerId = opcode & 0x0E;
             uint8_t regData0 = cpu->R[registerId];
             uint8_t regData1 = cpu->R[registerId + 1];

             cpu->PC = (regData0 << 4) | (regData1 & 0x0F);
         } else {
             uint8_t registerId = opcode & 0x0E;
             uint8_t regData0 = cpu->R[registerId];
             uint8_t regData1 = cpu->R[registerId + 1];

             uint8_t data = cpu->ROM[(regData0 << 4) | (regData1 & 0x0F)];

             cpu->R[registerId] = data & 0xF0;
             cpu->R[registerId + 1] = data & 0x0F;
         }

         cpu->PC++;
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

    // DEC
    {0x6, [](CPU4004* cpu, uint8_t opcode) { cpu->R[opcode & 0x0F]++; }},
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
    // LDM
    {0xD, [](CPU4004* cpu, uint8_t opcode) { cpu->AC = opcode & 0x0F; }},
    // CLC, STC
    {0xF,
     [](CPU4004* cpu, uint8_t opcode) {
         switch (opcode & 0x0F) {
         // CLC
         case 0x1:
             cpu->Carry = false;
             break;

         // STC
         case 0xA:
             cpu->Carry = true;
             break;
         }
     }},
};
