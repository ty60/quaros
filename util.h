#ifndef UTIL_H
#define UTIL_H

#include "types.h"
#include "interrupt.h"

void panic(const char *msg);
int atoi(const char *str);
void dump_regs(struct int_regs *regs);

#endif
