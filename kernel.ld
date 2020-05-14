ENTRY(_start)

SECTIONS {
    . = 0xc0100000 + SIZEOF_HEADERS;
    PROVIDE(text_start = .);
    .text : AT(ADDR(.text) - 0xc0000000) {
        *(.text)
    }
    PROVIDE(text_end = .);
    PROVIDE(rodata_start = .);
    .rodata : {
        *(.strings)
        *(.rodata)
        *(.rodata.*)
    }
    PROVIDE(rodata_end = .);
    PROVIDE(data_start = .);
    .data : {
        *(.data)
    }
    PROVIDE(data_end = .);
    PROVIDE(bss_start = .);
    .bss : {
        *(.bss)
    }
    PROVIDE(bss_end = .);
    PROVIDE(kernel_end = .);

    /DISCARD/ : {
        *(.eh_frame .note.gnu.build-id)
    }
}
