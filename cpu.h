#pragma once

#include <vector>
#include <stdint.h>

class RISCV_CPU{
private:
    static constexpr uint32_t ENTRY_ADDR  = 0;
    

    uint32_t regs[32];
    uint32_t pc;
    std::vector<uint8_t> memory;

public:
    volatile static constexpr uint32_t UART_ADDR = 0x10000000;
    
    RISCV_CPU();
    uint32_t fetch();
    void execute(uint32_t);
    void step();

    // test 
    void write_word(uint32_t addr, uint32_t inst);
    void dump_reg();
    
};