bits 32


%define VIRT_PHYS_OFFSET (0xc0000000)
%define VIRT_TO_PHYS(addr) (addr - VIRT_PHYS_OFFSET)


extern entry_pgdir
extern main

global _start
_start equ VIRT_TO_PHYS(start)

section .text
start:
    mov eax, cr4
    or eax, (1 << 4)
    mov cr4, eax
    mov eax, VIRT_TO_PHYS(entry_pgdir)
    mov cr3, eax ; load page directory address
    mov eax, cr0
    or eax, ((1 << 31) | (1 << 16)) ; enable paging and write protection
    mov cr0, eax
    mov eax, main
    jmp eax
fin:
    hlt
    jmp fin
