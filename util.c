#include "types.h"

void panic(const char *msg) {
    __asm__ volatile (
            "hlt\n\t"
            );
}


void *memset(void *s, int c, uint32_t n) {
    uint32_t i;
    for (i = 0; i < (n / sizeof(int)); i++) {
        *((int *)(s) + i) = c;
    }
    return s;
}
