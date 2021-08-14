#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "types.h"

#define T_IRQ_BASE 0x20
#define IRQ_SYSCALL 0x60


struct gate {
    uint16_t offset0; // offset 0:15
    uint16_t selector;
    uint8_t unused;
    uint8_t type: 4, s: 1, dpl: 2, p: 1;
    uint16_t offset1; // offset 16:31
} __attribute__ ((packed));


// Registers saved by `pushad`
struct regs {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t orig_esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
};


struct int_regs {
    struct regs saved_regs;
    uint16_t gs;
    uint16_t padding0;
    uint16_t fs;
    uint16_t padding1;
    uint16_t es;
    uint16_t padding2;
    uint16_t ds;
    uint16_t padding3;
    uint32_t vector_num;
    uint32_t error_code;
    uint32_t eip;
    uint16_t cs;
    uint16_t padding4;
    uint32_t eflags;
    // below are only present on privilege level change
    uint32_t esp;
    uint16_t ss;
    uint16_t padding5;
};


struct idt_desc {
    uint16_t size;
    uint16_t offset0;
    uint16_t offset1;
} __attribute__ ((packed));


void set_interrupt_gate(struct gate *gate_p, uint32_t offset, uint8_t dpl);
void set_trap_gate(struct gate *gate_p, uint32_t offset, uint8_t dpl);
void init_interrupt(void);

#endif
