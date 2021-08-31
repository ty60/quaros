#include "io.h"
#include "types.h"
#include "interrupt.h"
#include "proc.h"


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


void dump_regs(struct int_regs *regs) {
    print("gs: ");
    printnum(regs->gs);
    puts("");
    print("fs: ");
    printnum(regs->fs);
    puts("");
    print("es: ");
    printnum(regs->es);
    puts("");
    print("ds: ");
    printnum(regs->ds);
    puts("");
    print("cs: ");
    printnum(regs->cs);
    puts("");
}


void panic(const char *msg) {
    puts("I'm sorry Dave:");
    puts(msg);
    __asm__ volatile ("cli; hlt");
}
