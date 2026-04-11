#include <stdint.h>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <cstdlib>
#include <string>
#include <vector>

#define private public
#define main riscv_original_main
#include "main.cpp"
#undef main
#undef private

static uint32_t read_word(const RISCV_CPU& cpu, uint32_t addr) {
    return static_cast<uint32_t>(cpu.memory[addr]) |
           (static_cast<uint32_t>(cpu.memory[addr + 1]) << 8) |
           (static_cast<uint32_t>(cpu.memory[addr + 2]) << 16) |
           (static_cast<uint32_t>(cpu.memory[addr + 3]) << 24);
}

static void load_binary(RISCV_CPU& cpu, const std::vector<uint8_t>& bytes) {
    for (size_t offset = 0; offset < bytes.size(); offset += 4) {
        uint32_t word = 0;
        for (size_t i = 0; i < 4 && offset + i < bytes.size(); ++i) {
            word |= static_cast<uint32_t>(bytes[offset + i]) << (8 * i);
        }
        cpu.write_word(static_cast<uint32_t>(offset), word);
    }
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        std::cerr << "usage: emulator_test_harness <test.bin> [steps]\n";
        return 2;
    }
    int steps = argc == 3 ? std::atoi(argv[2]) : 2000;

    std::ifstream input(argv[1], std::ios::binary);
    if (!input) {
        std::cerr << "failed to open " << argv[1] << "\n";
        return 2;
    }

    std::vector<uint8_t> bytes{
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()};

    RISCV_CPU cpu;
    load_binary(cpu, bytes);

    for (int i = 0; i < steps; ++i) {
        cpu.step();
    }

    const uint32_t expected[] = {
        0x000001aeu,
        0xfffffffdu,
        0x7ffffffdu,
        0xffffff80u,
        0x00000080u,
        0xffff8001u,
        0x00008001u,
        0x89abcdefu,
        0xfffffffbu,
        0xfffffffdu,
        0xffffff80u,
        0xffff8001u,
        0x00000068u,
        0x600d600du,
    };

    bool ok = true;
    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); ++i) {
        uint32_t actual = read_word(cpu, 0x1000u + static_cast<uint32_t>(i * 4));
        bool pass = actual == expected[i];
        ok = ok && pass;
        std::cout << "OUT[" << i << "] expected=0x"
                  << std::hex << std::setw(8) << std::setfill('0') << expected[i]
                  << " actual=0x" << std::setw(8) << actual
                  << (pass ? " PASS" : " FAIL") << std::dec << "\n";
    }

    std::cout << "steps=" << steps << " pc=0x" << std::hex << cpu.pc << std::dec << "\n";
    return ok ? 0 : 1;
}
