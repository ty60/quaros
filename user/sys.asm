bits 32
section .text

%define INT_SYS 0x80

; These syscall number has be in sync with ../syscall.h
%define SYS_open 0
%define SYS_read 1
%define SYS_write 2
%define SYS_close 3
%define SYS_execv 4
%define SYS_fork 5
%define SYS_exit 6
%define SYS_wait 7

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
sys close
sys execv
sys fork
sys exit

; Since wait is an x86 instruction
; it can't be used as an symbol name.
; Use _wait instead.
global _wait
_wait:
    mov eax, SYS_wait
    int INT_SYS
    ret
