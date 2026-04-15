ENTRY(_start)

SECTIONS
{
    . = 0x0;
    .text : {
        *(.text.start)
        *(.text*)
    }
    .rodata : {
        *(.rodata*)
    }
    .data : {
        *(.data*)
        *(.sdata*)
    }
    
    . = ALIGN(4);
    _bss_start = .;
    .bss : {
        *(.bss*)
        *(.sbss*)
        *(COMMON)
    }
    . = ALIGN(8);
    _bss_end  = .;

    _heap_start = .;
    _heap_end = 0x00800000;

    _stack_top = 0x01000000;
}
