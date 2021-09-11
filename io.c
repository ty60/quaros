#include "uart.h"
#include "string.h"
#include "util.h"


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


#define MAX_BUF 4096
// buf_len: Length of data that is commited to buffer.
// edit_len: Length of data that is currently being edited and not commited to buffer.
unsigned int buf_len, edit_len;
unsigned int edit_i, write_i, read_i;
int circular_buf[MAX_BUF];

void write_circular_buf(int ch) {
    if (ch == DEL) {
        if (edit_len > 0) {
            edit_i = (edit_i - 1) % MAX_BUF;
            edit_len--;
        }
        return;
    }
    if (edit_len == MAX_BUF) {
        // TODO: Don't panic when buffer is full.
        panic("update_input_buf: Circular buffer full");
    }
    circular_buf[edit_i] = ch;
    edit_i = (edit_i + 1) % MAX_BUF;
    edit_len++;
    if (ch == '\n') {
        write_i = edit_i;
        buf_len = edit_len;
    }
}


// static inline int read_circular_buf(void) {
int read_circular_buf(void) {
    if (buf_len == 0) {
        // empty
        return -1;
    }
    int ret = circular_buf[read_i];
    read_i = (read_i + 1) % MAX_BUF;
    buf_len--;
    edit_len--;
    return ret;
}


int read_console(char *buf, size_t count) {
    int ch, i;
    i = 0;
    while (count-- > 0 && (ch = read_circular_buf()) >= 0) {
        buf[i++] = ch;
    }
    return i;
}
