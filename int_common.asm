bits 32
section .text

; Must be the same values as these are in memory.c
%define NULL_SEG (0)
%define  KERN_DATA_SEG (1)
%define  KERN_CODE_SEG (2)
%define  USER_DATA_SEG (3)
%define  USER_CODE_SEG (4)

extern trampoline

global int_common
global ret_to_int_site

int_common:
    ; Save registers of interrupt site.
    ; Could be both user space and kernel space.
    ; But since we use the big kernel lock,
    ; it will be user space only for now.
    push ds
    push es
    push fs
    push gs
    pushad
    ; Set kernel segment selectors.
    mov ax, (KERN_DATA_SEG << 3)
    mov ds, ax
    mov es, ax
    push esp ; pass pointer to current stack frame as argument
    call trampoline ; trampoline((struct int_regs *)&curr_stack);
    add esp, 4 ; ignore argument passed to trampoline
ret_to_int_site:
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 8 ; pop vector number and (dummy) error code
    iret
