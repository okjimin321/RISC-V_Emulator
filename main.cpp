#include <stdint.h>
#include <vector>
#include <iostream>

class RISCV_CPU{
private:
    uint32_t regs[32];
    uint32_t pc;
    std::vector<uint8_t> memory;

public:
    RISCV_CPU();
    uint32_t fetch();
    void execute(uint32_t);
    void step();

    // test 
    void write_word(uint32_t addr, uint32_t inst){
        for(int i = 0; i < 4; i++){
            memory[addr + i] = (inst >> (8 * i) & 0xff);
        }
    }
};

RISCV_CPU::RISCV_CPU(): pc { 0 }, memory(1024 * 1024, 0) {

    for(int i = 0; i < 32; i++){
        regs[i] = 0;
    }
}

void RISCV_CPU::step(){
        uint32_t inst = fetch();
        execute(inst);
}

uint32_t RISCV_CPU::fetch(){
    
    uint32_t inst = 0;
    for(int i = 0; i < 4; i++){
        inst |= (memory[pc + i]) << (i * 8); 
    }

    pc += 4;
    return inst;
}

void RISCV_CPU::execute(uint32_t inst){

    // extract opcode.
    uint8_t op = inst & 0x7f;

    switch (op){
        case 0x13:{ //immediate operation
 
            uint8_t funct = (inst >> 12) & 0x07;
            uint8_t  rs1  = (inst >> 15) & 0x1f;
            uint8_t  rd   = (inst >>  7) & 0x1f;
            int32_t imm  = static_cast<int32_t>(inst) >> 20;

            if(funct == 0x00){ //addi
                if(rd != 0) // regs[0] is not changable.
                    this->regs[rd] = this->regs[rs1] + imm;
                std::cout << this->regs[rd] << '\n';

            } else if(funct == 0x02){ //SLTI
                if(rd != 0)
                    this->regs[rd] = static_cast<int32_t>(regs[rs1]) < imm ? 1 : 0;

            } else if(funct == 0x03){ //SLTIU
                if(rd != 0)
                    this->regs[rd] = regs[rs1] < imm ? 1 : 0;
            }
            
            break;
        }

    }

}

int main(){
    RISCV_CPU cpu;

    cpu.step();
}