#include "memory.h"


extern char kernel_end[];

#define VRAM 0xb8000
#define COLOR 0x2a


char msg[] = "Hello world!";
char *msgs[] = {
    "Hello world!",
    "Hello kalloc!",
    "Hello kfree!!!",
};


void initbg(void) {
    int i;
    short *now = (short *)VRAM;
    for (i = 0; i < 2000; i++) {
        *now = COLOR << 8;
        now++;
    }
}


void print(char *msg, int n) {
    int i;
    char *now = (char *)VRAM;
    for (i = 0; i < n; i++) {
        *now = msg[i];
        now += 2;
    }
}


char *strcpy(char *dest, const char *src) {
    int i = 0;
    while (src[i]) {
        *dest++ = src[i++];
    }
    return dest;
}


int strlen(const char *s) {
    int ret;
    for (ret = 0; s[ret] != '\0'; ret++)
        ;
    return ret;
}


int main(void) {
    int i;
    char *dyn_mem[3];
    initbg();
    register_free_mem(kernel_end, (char *)(KERN_BASE & ~(0x00ffffff)) + (1024 * 1024 * 4));

    for (i = 0; i < 3; i++) {
        dyn_mem[i] = kmalloc();
        strcpy(dyn_mem[i], msgs[i]);
    }

    for (i = 0; i < 3; i++) {
        print(dyn_mem[i], strlen(dyn_mem[i]));
    }

    for (i = 0; i < 3; i++) {
        kfree(dyn_mem[i]);
    }
}
