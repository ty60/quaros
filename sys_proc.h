#ifndef SYS_PROC_H
#define SYS_PROC_H

#include "interrupt.h"

#define MAX_ARGC 8

int sys_fork(struct int_regs *frame);
int sys_execv(struct int_regs *frame);
int sys_exit(struct int_regs *frame);
int sys_wait(struct int_regs *frame);

#endif
