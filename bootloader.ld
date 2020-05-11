SECTIONS {
    . = 0x7c00;
    .text : {
        *(.text)
        *(.rodata)
        *(.data)
        *(.bss)
    }

    . = 0x7c00 + 510;
    .sign : {
        BYTE(0x55)
        BYTE(0xaa)
    }
}
