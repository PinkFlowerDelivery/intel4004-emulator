#include "cpu.h"
#include "mnemonics.h"
#include <cstdint>
#include <fmt/base.h>
#include <fstream>
#include <iostream>
#include <string>

std::array<uint8_t, 4096> readBinary(const std::string& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file)
        return {};

    std::array<uint8_t, 4096> buffer{};

    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

    return buffer;
}

// INFO: https://datasheets.chipdb.org/Intel/MCS-4/datashts/intel-4004.pdf
int main(int32_t argc, char* argv[]) {
    CPU4004 cpu;

    // i4004emu file.bin [-debug]

    std::string input;
    bool debug = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-debug") {
            debug = true;
        }

        if (input.empty()) {
            input = arg;
        }
    }

    if (input.empty()) {
        fmt::println(stderr, "usage: i4004emu main.bin [-debug]");
        return 1;
    }

    cpu.ROM = readBinary(input);

    while (cpu.PC < cpu.ROM.size()) {
        uint8_t opcode = cpu.fetch();

        fmt::print("{:x} ", opcode);

        if (mapOfMnemonics.find(opcode >> 4) == mapOfMnemonics.end()) {
            continue;
        }

        mapOfMnemonics[opcode >> 4](&cpu, opcode);
    }

    if (debug) {
        fmt::println("AC: {}", cpu.AC);
        fmt::println("Carry: {}", cpu.Carry);
        for (uint8_t i = 0; i < cpu.R.size(); i++) {
            fmt::println("R{}: {}", i, cpu.R[i]);
        }
    }
};
