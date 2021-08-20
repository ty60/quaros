#ifndef SYS_FILE_H
#define SYS_FILE_H

#include "interrupt.h"

int sys_open(struct int_regs *int_frame);
int sys_read(struct int_regs *int_frame);
int sys_write(struct int_regs *int_frame);

#endif
