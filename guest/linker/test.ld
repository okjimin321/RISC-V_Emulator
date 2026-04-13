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

    _stack_top = 0x01000000;
    _heap_end  = _stack_top - 0x00100000;
}
