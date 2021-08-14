ENTRY(main)

SECTIONS {
    . = SIZEOF_HEADERS;
    PROVIDE(text_start = .);
    .text : {
        *(.text)
        *(.data)
        *(.strings)
        *(.rodata)
        *(.rodata.*)
        *(.bss)
    }
    /DISCARD/ : {
        *(.eh_frame .note.gnu.build-id .note.gnu.property)
    }
}
