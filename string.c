#include "types.h"


char *strcpy(char *dest, const char *src) {
    int i = 0;
    while (src[i]) {
        *dest++ = src[i++];
    }
    *dest = '\0';
    return dest;
}


unsigned int strlen(const char *s) {
    unsigned int ret;
    for (ret = 0; s[ret] != '\0'; ret++)
        ;
    return ret;
}


int strcmp(const char *s1, const char *s2) {
    const char *p1, *p2;
    p1 = s1;
    p2 = s2;
    while (*p1 == *p2 && *p1 != '\0' && *p2 != '\0') {
        p1++;
        p2++;
    }

    if (*p1 == *p2 && *p1 == '\0') {
        return 0;
    }
    if (*p1 > *p2)
        return 1;
    else
        return -1;
}


void *memset(void *s, int c, uint32_t n) {
    uint32_t i;
    for (i = 0; i < (n / sizeof(int)); i++) {
        *((int *)(s) + i) = c;
    }
    return s;
}


void *memcpy(void *dest, const void *src, size_t n) {
    char *d;
    const char *s;
    d = dest;
    s = src;

    size_t i;
    for (i = 0; i < n; i++) {
        *d++ = *s++;
    }
    return dest;
}
