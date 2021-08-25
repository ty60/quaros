#ifndef UTIL_H
#define UTIL_H

#include "../types.h"

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2

char *strcpy(char *dest, const char *src);
unsigned int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
void *memset(void *s, int c, uint32_t n);
void *memcpy(void *dest, const void *src, size_t n);
int putchar(int fd, int c);
int printn(int fd, const char *s, size_t n);
int print(int fd, const char *s);
int puts(int fd, const char *s);
int printnum(int fd, int x);
int atoi(const char *s);

#endif
