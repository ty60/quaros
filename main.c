#include "memory.h"
#include "io.h"
#include "uart.h"
#include "lapic.h"
#include "ioapic.h"
#include "interrupt.h"
#include "fs.h"


extern char kernel_end[];


// This function HAS TO BE INLINED.
// Otherwise the new esp will be forgotten,
// when the old stack is restored with `leave`.
static inline void init_kstack(char *kstack) {
    __asm__ volatile (
            "mov esp, %0"
            :
            : "r" (kstack));
}


void bored(void) {
    while (1) {
    }
}


void show_banner(void) {
    struct file *banner_f = get_file("banner.txt");
    int i;
    for (i = 0; i < (int)banner_f->size; i++) {
        putchar(banner_f->data[i]);
    }
}


int main(void) {
    init_uart();
    puts("");
    puts("Boot quaros");

    puts("Setup kmalloc");
    register_free_mem(kernel_end, (char *)(KERN_LINK & ~(0x00ffffff)) + (1024 * 1024 * 4));
    init_kernel_memory();
    init_kstack(kmalloc());

    puts("Fill bss with zero");
    zero_out_bss();

    puts("Initialize segmentation");
    init_segmentation();

    puts("Initialize lapic");
    puts("Configure timer");
    init_lapic();
    puts("Disable pic");
    disable_pic();
    puts("Initialize ioapic");
    init_ioapic();

    puts("Initialize interrupt");
    init_interrupt();

    puts("Initialize file system");
    init_fs();

    puts("Welcome to...");
    show_banner();

    puts("I'm bored");
    bored();
}
