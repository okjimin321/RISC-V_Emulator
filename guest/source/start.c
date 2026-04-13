
__attribute__((naked, section(".text.start")))
void _start(void) {
    __asm__ volatile(
        "la sp, _stack_top\n"

        "la t0, _bss_start\n"
        "la t1, _bss_end\n"
        "1:\n"
        "bgeu t0, t1, 2f\n"
        "sw zero, 0(t0)\n"
        "addi t0, t0, 4\n"
        "j 1b\n"

        "2:\n"
        "call main\n"
        "3: j 3b\n"
    );

}