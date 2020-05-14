#include "types.h"


char *strcpy(char *dest, const char *src) {
    int i = 0;
    while (src[i]) {
        *dest++ = src[i++];
    }
    return dest;
}


unsigned int strlen(const char *s) {
    unsigned int ret;
    for (ret = 0; s[ret] != '\0'; ret++)
        ;
    return ret;
}


void *memset(void *s, int c, uint32_t n) {
    uint32_t i;
    for (i = 0; i < (n / sizeof(int)); i++) {
        *((int *)(s) + i) = c;
    }
    return s;
}
