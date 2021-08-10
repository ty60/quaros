#ifndef STRING_H
#define STRING_H

#include "types.h"

void *memset(void *s, int c, uint32_t n);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
unsigned int strlen(const char *s);
void *memcpy(void *dest, const void *src, size_t n);


#endif
