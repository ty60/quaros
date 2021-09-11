#include "sys.h"


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


int putchar(int fd, int c) {
    write(fd, &c, 1);
    return c;
}


int printn(int fd, const char *s, size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        putchar(fd, s[i]);
    }
    return n;
}


int print(int fd, const char *s) {
    unsigned int i;
    for (i = 0; i < strlen(s); i++) {
        putchar(fd, s[i]);
    }
    return strlen(s);
}


int puts(int fd, const char *s) {
    print(fd, s);
    putchar(fd, '\n');
    return strlen(s) + 1;
}


int printnum(int fd, int x) {
    if (x == 0) {
        putchar(fd, '0');
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
        putchar(fd, tmp[i]);
    }
    return 1;
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


int read_line(int fd, char *buf, int buf_size) {
    int i = 0;
    while (i < buf_size) {
        char ch;
        if (read(fd, &ch, 1) <= 0) {
            // TODO: Implement sleep() in kernel
            continue;
        }
        buf[i++] = ch;
        if (ch == '\n')
            break;
    }
    return i;
}
