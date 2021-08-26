bits 32
section .text

%define INT_SYS 0x80

; These syscall number has be in sync with ../syscall.h
%define SYS_open 0
%define SYS_read 1
%define SYS_write 2
%define SYS_close 3
%define SYS_exec 4
%define SYS_fork 5
%define SYS_exit 6

%macro sys 1
global %1
%1:
    mov eax, SYS_%1
    int INT_SYS
    ret
%endmacro

sys open
sys read
sys write
sys exec
sys fork
sys exit
sys close
