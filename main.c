#include "memory.h"


extern char kernel_end[];

#define VRAM 0xb8000
#define COLOR 0x2a


char msg[] = "Hello world!";


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


// This function HAS TO BE INLINED.
// Otherwise the new esp will be forgotten,
// when the old stack is restored with `leave`.
static inline void init_kstack(char *kstack) {
    __asm__ volatile (
            "mov esp, %0"
            :
            : "r" (kstack));
}


int main(void) {
    register_free_mem(kernel_end, (char *)(KERN_LINK & ~(0x00ffffff)) + (1024 * 1024 * 4));
    init_kernel_memory();
    init_kstack(kmalloc());

    initbg();
    print(msg, sizeof(msg));

}
