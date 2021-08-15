bits 32
section .text

%define INT_SYS 0x80

; These syscall number has be in sync with ../syscall.h
%define SYS_open 0
%define SYS_read 1
%define SYS_write 2
%define SYS_exec 3
%define SYS_fork 4

%macro sys 1
global %1
%1:
    mov eax, SYS_%1
    int INT_SYS
    ret
%endmacro

sys open
sys write
sys read
