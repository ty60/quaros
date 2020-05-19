#include "interrupt.h"
#include "types.h"
#include "util.h"
#include "memory.h"
#include "lapic.h"
#include "ioapic.h"
#include "io.h"
#include "asm.h"

#define NUM_GATES 256

#define GATE_TYPE_INTERRUPT 0xe
#define GATE_TYPE_TRAP 0xf


struct gate idt[NUM_GATES];

extern uint32_t vector_table[];

// interrupt gate disables interrupt
void set_interrupt_gate(struct gate *gate_p, uint32_t offset, uint8_t dpl) {
    gate_p->offset0 = offset & 0xffff;
    gate_p->selector = KERN_CODE_SEG << 3;
    gate_p->unused = 0;
    gate_p->type = GATE_TYPE_INTERRUPT;
    gate_p->s = 0;
    gate_p->dpl = dpl;
    gate_p->p = 1;
    gate_p->offset1 = (offset >> 16) & 0xffff;
}


// trap gate is same as interrupt gate but interrupt is enabled
void set_trap_gate(struct gate *gate_p, uint32_t offset, uint8_t dpl) {
    gate_p->offset0 = offset & 0xffff;
    gate_p->selector = KERN_CODE_SEG << 3;
    gate_p->unused = 0;
    gate_p->type = GATE_TYPE_TRAP;
    gate_p->s = 0;
    gate_p->dpl = dpl;
    gate_p->p = 1;
    gate_p->offset1 = (offset >> 16) & 0xffff;
}


unsigned int ticks = 0;


void trampoline(struct int_regs *regs) {
    int irq = regs->vector_num & 0xff;
    if (irq == T_IRQ_BASE + IRQ_TIMER) {
        ticks++;
        puts("timer");
        eoi();
    } else if (irq == T_IRQ_BASE + IRQ_SPURIOUS) {
        eoi();
    } else if (irq == T_IRQ_BASE + IRQ_ERR) {
        panic("Internal error in lapic");
    } else {
        panic("Unknown irq");
    }
}


struct idt_desc idtr;

void init_idt(void) {
    int i;
    for (i = 0; i < NUM_GATES; i++) {
        set_interrupt_gate(&idt[i], vector_table[i], DPL_KERN);
    }
    set_trap_gate(&idt[T_IRQ_BASE + IRQ_SYSCALL],
                  (uint32_t)&vector_table[T_IRQ_BASE + IRQ_SYSCALL], DPL_USER);

    idtr.size = sizeof(idt) - 1;
    idtr.offset0 = (uint32_t)idt & 0xffff;
    idtr.offset1 = (uint32_t)idt >> 16 & 0xffff;
    lidt(&idtr);
}


void init_interrupt(void) {
    init_idt();
    sti();
}
