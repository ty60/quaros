#include "uart.h"
#include "string.h"


int putchar(int c) {
    uart_write_byte((uint8_t)c);
    return c;
}


int puts(const char *s) {
    unsigned int i;
    for (i = 0; i < strlen(s); i++) {
        putchar(s[i]);
    }
    putchar('\n');
    return 1;
}
