bits 32


%define NULL_SEG (0)
%define KERN_DATA_SEG (1)
%define KERN_CODE_SEG (2)
%define USER_DATA_SEG (3)
%define USER_CODE_SEG (4)


global reload_segment_regs
segment .text

reload_segment_regs:
    push ebp
    mov ebp, esp
    jmp KERN_CODE_SEG << 3:reload_cs
reload_cs:
    xor eax, eax
    mov gs, ax
    mov fs, ax
    mov ax, KERN_DATA_SEG << 3
    mov ds, ax
    mov es, ax
    mov ss, ax
    leave
    ret
