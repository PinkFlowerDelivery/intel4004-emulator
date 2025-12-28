#include "cpu.h"
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <unordered_map>

std::unordered_map<uint8_t, std::function<void(CPU4004 *, uint8_t)>>
    mapOfMnemonics = {
        // NOP
        {0x0, [](CPU4004 *cpu, uint8_t opcode) { cpu->PC++; }},
        // JUN
        {0x4,
         [](CPU4004 *cpu, uint8_t opcode) {
           uint8_t nibbles[3];
           nibbles[0] = opcode & 0x0F;

           uint8_t nextOpcode = cpu->fetch();

           nibbles[1] = nextOpcode >> 4;
           nibbles[2] = nextOpcode & 0x0F;

           cpu->PC = (nibbles[0] << 8) | (nibbles[1] << 4) | nibbles[2];
         }},
        // DEC
        {0x6, [](CPU4004 *cpu, uint8_t opcode) { cpu->R[opcode & 0x0F]++; }},
        // ADD
        {0x8,
         [](CPU4004 *cpu, uint8_t opcode) {
           cpu->AC += cpu->R[opcode & 0x0F] + cpu->Carry;
           cpu->Carry = false;
           if (cpu->AC & 0xF0) {
             cpu->AC &= 0xF;
             cpu->Carry = true;
           }
         }},
        // SUB
        {0x9,
         [](CPU4004 *cpu, uint8_t opcode) {
           cpu->AC += ~cpu->R[opcode & 0xF] + !cpu->Carry;
           cpu->Carry = false;

           if (cpu->AC & 0xF0) {
             cpu->AC &= 0xF;
             cpu->Carry = true;
           }
         }},
        // LD
        {0xA,
         [](CPU4004 *cpu, uint8_t opcode) { cpu->AC = cpu->R[opcode & 0x0F]; }},
        // XCH
        {0xB,
         [](CPU4004 *cpu, uint8_t opcode) {
           uint8_t temp = cpu->AC;
           cpu->AC = cpu->R[opcode & 0x0F];
           cpu->R[opcode & 0x0F] = temp;
         }},
        // LDM
        {0xD, [](CPU4004 *cpu, uint8_t opcode) { cpu->AC = opcode & 0x0F; }},
        // CLC, STC
        {0xF,
         [](CPU4004 *cpu, uint8_t opcode) {
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
