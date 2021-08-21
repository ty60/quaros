#ifndef SYS_PROC_H
#define SYS_PROC_H

#include "interrupt.h"

int sys_fork(struct int_regs *frame);
int sys_exec(struct int_regs *frame);
int sys_exit(struct int_regs *frame);

#endif
