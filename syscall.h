#ifndef SYSCALL_H
#define SYSCALL_H

#include "interrupt.h"

enum SYS_nums {
    SYS_open,
    SYS_read,
    SYS_write,
    SYS_exec,
    SYS_fork,
    SYS_exit,

    NUM_SYSCALL,
};

int handle_syscall(struct int_regs *frame);
int read_user_mem(void *addr, uint32_t *data);
int read_syscall_arg(void *esp, int argnum, uint32_t *data);

#endif
