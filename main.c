#include "memory.h"
#include "io.h"
#include "uart.h"
#include "lapic.h"
#include "ioapic.h"


extern char kernel_end[];

char msg[] = "Hello world!";


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

    zero_out_bss();

    init_segmentation();

    initlapic();
    disablepic();
    init_ioapic();

    init_uart();
    puts(msg);
}
