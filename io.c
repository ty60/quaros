#include "uart.h"
#include "string.h"


int putchar(int c) {
    uart_write_byte((uint8_t)c);
    return c;
}


int printn(const char *s, size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        putchar(s[i]);
    }
    return n;
}


int print(const char *s) {
    unsigned int i;
    for (i = 0; i < strlen(s); i++) {
        putchar(s[i]);
    }
    return strlen(s);
}


int puts(const char *s) {
    print(s);
    putchar('\n');
    return strlen(s) + 1;
}


int printnum(int x) {
    if (x == 0) {
        putchar('0');
        return 1;
    }

    int num_digits = 0;
    char tmp[11];
    while (x > 0) {
        int t = x % 10;
        tmp[num_digits++] = '0' + (char)t;
        x /= 10;
    }

    int i;
    for (i = num_digits - 1; i >= 0; i--) {
        putchar(tmp[i]);
    }
    return 1;
}
