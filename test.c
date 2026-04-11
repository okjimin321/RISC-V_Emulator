typedef unsigned int u32;
typedef signed int s32;
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;

#define OUT32  ((volatile u32 *)0x1000u)
#define MEM8   ((volatile u8  *)0x1100u)
#define MEM16  ((volatile u16 *)0x1200u)
#define MEM32  ((volatile u32 *)0x1300u)

__attribute__((noinline))
static s32 mix(s32 a, s32 b) {
    s32 r = a + b;
    r = r - b;
    r = r ^ 0x55;
    r = r | 0x100;
    r = r & 0x1ff;
    return r;
}

__attribute__((noinline))
static s32 signed_branch(s32 x) {
    if (x < 0) {
        return x >> 1;
    }
    return x << 1;
}

__attribute__((noinline))
static u32 unsigned_branch(u32 x) {
    if (x >= 10u) {
        return x >> 1;
    }
    return x + 1u;
}

__attribute__((noinline))
static s32 callee(s32 x) {
    return (x + 7) - 3;
}

__attribute__((noinline))
static s32 asm_sub(s32 a, s32 b) {
    s32 out;
    __asm__ volatile("sub %0, %1, %2" : "=r"(out) : "r"(a), "r"(b));
    return out;
}

__attribute__((noinline))
static s32 asm_sra(s32 a, u32 shamt) {
    s32 out;
    __asm__ volatile("sra %0, %1, %2" : "=r"(out) : "r"(a), "r"(shamt));
    return out;
}

__attribute__((noinline))
static s32 asm_lb(void) {
    s32 out;
    __asm__ volatile("lb %0, 0(%1)" : "=r"(out) : "r"(MEM8) : "memory");
    return out;
}

__attribute__((noinline))
static s32 asm_lh(void) {
    s32 out;
    __asm__ volatile("lh %0, 0(%1)" : "=r"(out) : "r"(MEM16) : "memory");
    return out;
}

void entry(void) {
    MEM8[0] = 0x80u;
    MEM8[1] = 0x7fu;
    MEM16[0] = 0x8001u;
    MEM32[0] = 0x89abcdefu;

    OUT32[0] = (u32)mix(-5, 12);
    OUT32[1] = (u32)signed_branch(-5);
    OUT32[2] = unsigned_branch(0xfffffffbu);
    OUT32[3] = (u32)((volatile s8 *)MEM8)[0];
    OUT32[4] = (u32)((volatile u8 *)MEM8)[0];
    OUT32[5] = (u32)((volatile s16 *)MEM16)[0];
    OUT32[6] = (u32)((volatile u16 *)MEM16)[0];
    OUT32[7] = MEM32[0];
    OUT32[8] = (u32)asm_sub(7, 12);
    OUT32[9] = (u32)asm_sra(-5, 1);
    OUT32[10] = (u32)asm_lb();
    OUT32[11] = (u32)asm_lh();
    OUT32[12] = (u32)callee(100);
    OUT32[13] = 0x600d600du;
}

__attribute__((naked, section(".text.start")))
void _start(void) {
    __asm__ volatile(
        "lui sp, 0x80\n"
        "call entry\n"
        "1: j 1b\n"
    );
}
