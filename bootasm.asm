bits 16


extern bootc


%define nullentry dq 0
%define RW (1 << 1)
%define Ex (1 << 3)


%macro gdtentry 3
    dw (%3 & 0xffff) ; limits[0:15]
    dw (%2 & 0xffff) ; base[0:15]
    db ((%2 >> 16) & 0xff) ; base[16:23]
    db (0x90 | %1) ; access byte
    db (0xc0 | ((%3 >> 16) & 0x0f)) ; (flags | limit[16:19])
    db ((%2 >> 24) & 0xff)
%endmacro


bootasm:
    cli
    xor ax, ax
    mov ss, ax
    mov ds, ax

wait_kdb1:
    in al, 0x64
    test al, 0x2
    jnz wait_kdb1

    mov al, 0xd1
    out 0x64, al

wait_kdb2:
    in al, 0x64
    test al, 0x2
    jnz wait_kdb2

    mov al, 0xdf
    out 0x60, al

wait_kdb3:
    in al, 0x64
    test al, 0x2
    jnz wait_kdb3
; Done enabling A20

; load gdtdecs
    lgdt [gdtdesc]
; Configure cr0 for protected mode
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp (2 << 3):protected ; long jump to enable protected mode

bits 32
protected:
    xor eax, eax
    mov gs, eax
    mov fs, eax
    mov eax, (1 << 3)
    mov ds, eax
    mov es, eax
    mov ss, eax
    mov esp, bootasm
    call bootc

fin:
    hlt
    jmp fin


align 4
gdt:
    nullentry
    gdtentry RW, 0x00000000, 0xfffff
    gdtentry Ex, 0x00000000, 0xfffff

gdtdesc:
    dw (8 * 3 - 1)
    dd gdt
