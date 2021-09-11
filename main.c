#include "memory.h"
#include "io.h"
#include "uart.h"
#include "lapic.h"
#include "ioapic.h"
#include "interrupt.h"
#include "fs.h"
#include "proc.h"
#include "util.h"
#include "asm.h"


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
        puts("I'm Sorry Dave");
        int i;
        for (i = 0; i < 10000000; i++) ;
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

    // bss has to be zeroed out first,
    // before any of the global variables are actually used.
    puts("Fill bss with zero");
    zero_out_bss();

    puts("Setup kmalloc");
    // entry_pgdir maps 4MB
    register_free_mem(kernel_end,
                      (char *)(KERN_LINK & ~(0x00ffffff)) + (1024 * 1024 * 4));
    init_kernel_memory();
    init_kstack(kmalloc() + PGSIZE);

    puts("Initialize segmentation");
    init_segmentation();

    puts("Setup rest of kmalloc");
    // init_kernel_memory will map [KERN_BASE: KERN_TOP) to physical memory
    register_free_mem((char *)(KERN_LINK & (~0x00ffffff)) + (1024 * 1024 * 4),
                      (char *)(KERN_TOP));

    puts("Initialize lapic");
    puts("Configure timer");
    init_lapic();
    puts("Disable pic");
    disable_pic();
    puts("Initialize ioapic");
    init_ioapic();
    enable_ioapic(IRQ_SERIAL);

    puts("Initialize interrupt");
    init_interrupt();

    puts("Initialize file system");
    init_fs();
    puts("Initialize dev files");
    init_dev_file();

    puts("Initialize tasks");
    init_tasks();

    puts("Create init task");
    create_init_task();

    puts("Welcome to...");
    show_banner();

    // puts("Become scheduler");
    schedule();

    bored();

    return 0; // means nothing
}
