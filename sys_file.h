#ifndef SYS_FILE_H
#define SYS_FILE_H

#include "interrupt.h"

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2

int sys_open(struct int_regs *int_frame);
int sys_read(struct int_regs *int_frame);
int sys_write(struct int_regs *int_frame);
int sys_close(struct int_regs *int_frame);

#endif
