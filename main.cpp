#include <stdint.h>
#include <vector>
#include <stdio.h>
#include <iostream>

#define ENTRY_ADDR 0

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

    // test
    void dump_reg(){
        printf("\n================ Register Dump =================\n");
        for(int i = 0; i < 32; i++){
            printf("x%-2d: %-11d (0x%08x)  ", i, (int32_t)regs[i], regs[i]);
            if((i + 1) % 4 == 0){
                printf("\n");
            }
        }
        printf("==============================================\n");
    }
    
};

RISCV_CPU::RISCV_CPU(): pc { ENTRY_ADDR }, memory(1024 * 1024, 0) {

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
        inst |= (static_cast<uint32_t>(memory[pc + i]) << (i * 8)); 
    }

    pc += 4;
    return inst;
}

void RISCV_CPU::execute(uint32_t inst){

    // extract opcode.
    uint8_t op = inst & 0x7f;

    switch (op){
        case 0x13:{ // I-Type
 
            uint8_t funct = (inst >> 12) & 0x07;
            uint8_t  rs1  = (inst >> 15) & 0x1f;
            uint8_t  rd   = (inst >>  7) & 0x1f;
            int32_t imm  = static_cast<int32_t>(inst) >> 20;

            uint32_t result = 0;

            switch (funct){

                case 0x00: result = this->regs[rs1] + imm;                              break; // ADDI
                case 0x01: result = this->regs[rs1] << (imm & 0x1f);                    break; // SLLI
                case 0x02: result = static_cast<int32_t>(regs[rs1]) < imm  ? 1 : 0;     break; // SLTI
                case 0x03: result = regs[rs1] < static_cast<uint32_t>(imm) ? 1 : 0;     break; // SLTIU
                case 0x04: result = regs[rs1] ^ imm;                                    break; // XORI
                case 0x05:{ // SRLI

                    bool isArithmetic = (static_cast<uint32_t>(imm) >> 10) & 0x1;
                    uint8_t shamt = imm & 0x1f;
                    if (isArithmetic) {
                        result = static_cast<int32_t>(regs[rs1]) >> shamt;
                    } else {
                        result = regs[rs1] >> shamt;
                    }
                    break;
                }
                case 0x06: result = regs[rs1] | imm;                                    break; // ORI
                case 0x07: result = regs[rs1] & imm;                                    break; // ANDI
                default: return;
            }
            
            if(rd != 0){ // reg[0] is not changable
                regs[rd] = result;
            }

            break;
        }
        case 0x33: {// R-Type
            
            uint8_t rd     = (inst >> 7)  & 0x1f;
            uint8_t funct3 = (inst >> 12) & 0x07;
            uint8_t rs1    = (inst >> 15) & 0x1f;
            uint8_t rs2    = (inst >> 20) & 0x1f;
            uint8_t funct7 = (inst >> 25) & 0x7f;

            uint32_t result = 0;

            switch (funct3){
                case 0x00:{ // add and sub

                    if(funct7 == 0x00){
                        result = regs[rs1] + regs[rs2];
                    } else if(funct7 == 0x20){
                        result = regs[rs1] - regs[rs2];
                    } else{
                        return;
                    }
                    break;
                }
                case 0x01: result = regs[rs1] << (regs[rs2] & 0x1f);                                             break; // SLL
                case 0x02: result = (static_cast<int32_t>(regs[rs1]) < static_cast<int32_t>(regs[rs2]))? 1 : 0;  break; // SLT
                case 0x03: result = (regs[rs1] < regs[rs2]) ? 1 : 0;                                              break; // SLTU
                case 0x05:{ // SRL and SRA

                    if(funct7 == 0x00){ 
                        result = regs[rs1] >> (regs[rs2] & 0x1f);
                    } else if(funct7 == 0x20){ // Arithmetic
                        result = static_cast<int32_t>(regs[rs1]) >> (regs[rs2] & 0x1f);
                    } else{
                        return;
                    }
                    break;
                }
                case 0x04: result = regs[rs1] ^ regs[rs2];                                              break; // XOR
                case 0x06: result = regs[rs1] | regs[rs2];                                              break; // OR
                case 0x07: result = regs[rs1] & regs[rs2];                                              break; // AND
                default: return;
            }

            if(rd != 0){
                regs[rd] = result;
            }
            break;
        }
        
        case 0x6f:{ // JAL Operation
            
            uint8_t rd         = (inst >> 7)  & 0x1f;
            int32_t imm20      = (inst >> 31) & 0x1;
            int32_t imm10_1    = (inst >> 21) & 0x3ff;
            int32_t imm11      = (inst >> 20) & 0x1;
            int32_t imm19_12   = (inst >> 12) & 0xff;

            int32_t offset = (imm20 << 20) | (imm10_1 << 1) | (imm11 << 11) | (imm19_12 << 12);
            if(offset & (1 << 20)){
                offset |= 0xffe00000; // Arithmetic
            }

            // save Return address
            if(rd != 0)
                this->regs[rd] = this->pc;

            // Jump
            this->pc = (pc - 4) + offset; 
            break;  
        }
        
        case 0x67: { // JALR Operation 
            uint8_t rd      = (inst >> 7)  & 0x1f;
            uint8_t funct3  = (inst >> 12) & 0x07;
            uint8_t rs1     = (inst >> 15) & 0x1f;

            int32_t offset  = (static_cast<int32_t>(inst) >> 20);
            uint32_t source = regs[rs1];

            if(funct3 == 0){
                // save Return address
                if(rd != 0)
                    this->regs[rd] = this->pc;

                // Jump
                this->pc = (source + offset) & ~0x1; 
            } else{
                return;
            }

            break;
        }
        
        case 0x63:{ // B-type
            
            uint8_t  rs1     = (inst >> 15) & 0x1f;
            uint8_t  rs2     = (inst >> 20) & 0x1f;
            uint8_t  funct3  = (inst >> 12) & 0x7;
            uint32_t imm11   = (inst >> 7)  & 0x1;
            uint32_t imm4_1  = (inst >> 8)  & 0xf;
            uint32_t imm10_5 = (inst >> 25) & 0x3f;
            uint32_t imm12   = (inst >> 31) & 0x1;

            uint32_t offset = (imm4_1 << 1) | (imm10_5 << 5) | (imm11 << 11) | (imm12 << 12);
            if(offset & (1 << 12)){
                offset |= 0xffffe000;
            }

            bool jump = false;
            switch(funct3){
                case 0x0:{ // BEQ
                    if(regs[rs1] == regs[rs2]) 
                        jump = true;
                    break;
                }
                case 0x01:{ // BNE
                    if(regs[rs1] != regs[rs2])
                        jump = true;
                    break;
                }
                case 0x04:{ // BLT
                    if(static_cast<int32_t>(regs[rs1]) < static_cast<int32_t>(regs[rs2]))
                        jump = true;
                    break;
                }
                case 0x05:{ // BGE
                    if(static_cast<int32_t>(regs[rs1]) >= static_cast<int32_t>(regs[rs2]))
                        jump = true;
                    break;
                }
                case 0x06:{ // BLTU
                    if(regs[rs1] < regs[rs2])
                        jump = true;
                    break;
                }
                case 0x07:{ // BGEU
                    if(regs[rs1] >= regs[rs2])
                        jump = true;
                    break;
                }

                
            }
            if(jump)
                this->pc = (this->pc - 4) + offset;

            break;
        }
        case 0x03:{ // Load Operation

            int32_t imm     = static_cast<int32_t>(inst) >> 20;
            uint8_t rs1     = (inst >> 15) & 0x1f;
            uint8_t funct3  = (inst >> 12) & 0x07;
            uint8_t rd      = (inst >> 7)  & 0x1f;
            
            uint32_t addr   = regs[rs1] + imm;
            uint32_t result = 0;

            switch(funct3){
                case 0x00:{ // LB
                    result = static_cast<int32_t>(static_cast<int8_t>(memory[addr]));
                    break;
                }
                case 0x01:{ // LH
                    result = memory[addr] | memory[addr + 1] << 8;
                    result = static_cast<int32_t>(static_cast<int16_t>(result));
                    break;
                }
                case 0x02:{ // LW
                    result = memory[addr] | static_cast(uint32_t)(memory[addr + 1]) << 8 
                            | static_cast<uint32_t>(memory[addr + 2]) << 16 
                            | static_cast<uint32_t>(memory[addr + 3]) << 24;
                    break;
                }
                case 0x04:{ // LBU
                    result = memory[addr];
                    break;
                }
                case 0x05:{ // LHU
                    result = memory[addr] | static_cast<uint32_t>(memory[addr + 1]) << 8;
                    break;
                }
            }

            if(rd != 0){
                regs[rd] = result;
            }
            break;
        }
        case 0x23:{ // Store

            uint32_t imm11_5     = (inst >> 25);
            uint32_t imm4_0      = (inst >> 7) & 0x1f;
            uint8_t  rs1         = (inst >> 15) & 0x1f;
            uint8_t  rs2         = (inst >> 20) & 0x1f;
            uint8_t  funct3      = (inst >> 12) & 0x07;
            
            uint32_t offset     = (imm4_0) | (imm11_5 << 5);
            if(offset & (1 << 11)){
                offset |= 0xfffff000;
            }
            uint32_t addr       = regs[rs1] + offset;
            uint32_t result     = 0;

            switch(funct3){
                case 0x00:{
                    result = (regs[rs2] & 0xff);
                    break;
                }
                case 0x01:{
                    result = (regs[rs2] & 0xffff);
                    break;
                }
                case 0x02:{
                    result = (regs[rs2] & 0xffffffff);
                    break;
                }
            }

            for(int i = 0; i < (1 << funct3); i++){
                memory[addr + i] = (result >> (i * 8)) & 0xff;
            }
            break;
        }
        case 0x37:{ //LUI
            
            uint32_t imm = inst & 0xfffff000;
            uint8_t  rd  = (inst >> 7) &  0x1f;

            if(rd != 0)
                regs[rd] = imm; 
            break;
        }
        case 0x17:{

            uint32_t imm = inst & 0xfffff000;
            uint8_t  rd  = (inst >> 7) & 0x1f;

            if(rd != 0)
                regs[rd] = (pc - 4) + imm;
            break;
        }
    }

}

int main() {
    RISCV_CPU cpu;

    std::cout << "--- RISC-V Emulator Test Start ---" << std::endl;

    // 1. x1 = x0 + 10 (addi x1, x0, 10)
    // 기대 결과: x1 = 10 (0x0000000a)
    cpu.write_word(0, 0x00A00093); 

    // 2. x2 = x0 - 5 (addi x2, x0, -5)
    // -5는 12비트 즉시값으로 0xFFB입니다.
    // 기대 결과: x2 = -5 (0xfffffffb) -> 부호 확장 확인
    cpu.write_word(4, 0xFFB00113); 

    // 3. x3 = x2 >> 1 (srai x3, x2, 1)
    // -5(111...1011)를 오른쪽으로 1칸 산술 시프트
    // 기대 결과: x3 = -3 (0xfffffffd) -> SRAI 로직 확인
    cpu.write_word(8, 0x40115193); 

    // 4. x4 = (x2 < 10) ? 1 : 0 (slti x4, x2, 10)
    // -5 < 10 은 참이므로 1이 저장되어야 함
    // 기대 결과: x4 = 1 (0x00000001) -> Signed 비교 확인
    cpu.write_word(12, 0x00A12213);

    // 5. x5 = (unsigned x2 < 10) ? 1 : 0 (sltiu x5, x2, 10)
    // 0xfffffffb(42억...) < 10 은 거짓이므로 0이 저장되어야 함
    // 기대 결과: x5 = 0 (0x00000000) -> Unsigned 비교 확인
    cpu.write_word(16, 0x00A13293);

    // 총 5개의 명령어를 실행합니다.
    for (int i = 0; i < 5; i++) {
        std::cout << "\n[Step " << i + 1 << "]" << std::endl;
        cpu.step();
        cpu.dump_reg(); // 매 단계마다 레지스터 변화를 찍어봅니다.
    }

    std::cout << "\n--- Test Completed ---" << std::endl;

    return 0;
}