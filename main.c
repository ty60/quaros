#include "memory.h"
#include "io.h"
#include "uart.h"
#include "lapic.h"
#include "ioapic.h"
#include "trap.h"


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


int main(void) {
    init_uart();
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

    puts("I'm bored");
    bored();
}
