#ifndef IO_H
#define IO_H

#include "types.h"

int putchar(int c);
int print(const char *s);
int puts(const char *s);
int printnum(int x);
int printn(const char *s, size_t n);
void write_circular_buf(int ch);
int read_console(char *buf, size_t count);

#endif
