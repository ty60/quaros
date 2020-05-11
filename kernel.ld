ENTRY(_start)

SECTIONS {
    . = 0x100000 + SIZEOF_HEADERS;
    .text : {
        *(.text)
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss)
    }

    /DISCARD/ : {
        *(.eh_frame .note.gnu.build-id)
    }
}
