#pragma once

#include <vector>
#include <stdint.h>
#include <chrono>
#include <queue>

class RISCV_CPU{
private:
    static constexpr uint32_t ENTRY_ADDR  = 0;

    uint32_t regs[32];
    uint32_t pc;

    // Timer
    std::chrono::system_clock::time_point bootTime;

    // TODO (Seperate CPU/BUS/MEMORY)
    std::vector<uint8_t> memory;
    std::vector<uint32_t> frameBuffer;
    std::queue<uint32_t> keyQueue;
    
public:
    static constexpr uint32_t UART_ADDR       = 0x10000000;
    static constexpr uint32_t FRAMEBUF_ADDR   = 0x20000000;
    static constexpr uint32_t TIMER_ADDR      = 0x30000000;
    static constexpr uint32_t KEY_VALUE_ADDR  = 0x40000000;
    static constexpr uint32_t KEY_STATUS_ADDR = 0x40000004;
    
    RISCV_CPU();
    uint32_t fetch();
    void execute(uint32_t);
    void step();

    // test 
    void write_word(uint32_t addr, uint32_t inst);
    void dump_reg();
    void put_keyBoard(uint32_t value){
        keyQueue.push(value);
    }
    std::vector<uint32_t>& getFrameBuffer(){
        return frameBuffer;
    }
    
};