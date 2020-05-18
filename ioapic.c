#include "types.h"
#include "util.h"
#include "trap.h"


#define IOAPIC 0xfec00000
#define IA_REDIR_BASE 0x10
#define IA_MASK (1 << 16)

volatile uint32_t *ioapic = (uint32_t *)IOAPIC;

uint32_t read_ioapic(uint32_t reg) {
    ioapic[0] = reg & 0xff;
    return ioapic[4];
}


void write_ioapic(uint32_t reg, uint32_t data) {
    ioapic[0] = reg & 0xff;
    ioapic[4] = data;
}


void disable_ioapic(uint32_t irq) {
    write_ioapic(IA_REDIR_BASE + irq * 2, IA_MASK);
    write_ioapic(IA_REDIR_BASE + irq * 2 + 1, 0);
}


void enable_ioapic(uint32_t irq) {
    // Map IRQ #irq to vector T_IRQ_BASE + irq
    // edge-triggered, active high, enabled
    // Route interrupt to cpu with lapic id 0
    write_ioapic(IA_REDIR_BASE + irq * 2, T_IRQ_BASE + irq);
    write_ioapic(IA_REDIR_BASE + irq * 2 + 1, T_IRQ_BASE + irq);
}


void init_ioapic(void) {
    uint32_t max_redir_entry, i;
    max_redir_entry = (read_ioapic(0x01) >> 16) & 0xff;
    for (i = 0; i <= max_redir_entry; i++) {
        disable_ioapic(i);
    }
}
