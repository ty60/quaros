#include "io.h"
#include "types.h"

void panic(const char *msg) {
    puts(msg);
    __asm__ volatile ("hlt");
}


int atoi(const char *s) {
    const char *p = s;
    int ret = 0;
    int base = 1;

    while (*(p + 1) != '\x00') {
        p++;
    }

    while (p != s) {
        ret += base * (*p - '0');
        base *= 10;
        p--;
    }
    if (*p == '-') {
        ret *= -1;
    } else {
        ret += base * (*p - '0');
    }
    return ret;
}
