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

    .bss : {
        *(.bss*)
        *(.sbss*)
        *(COMMON)
    }
}
