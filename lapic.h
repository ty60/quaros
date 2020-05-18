#ifndef LAPIC_H
#define LAPIC_H

#define IRQ_TIMER 0
#define IRQ_SPURIOUS 7
#define IRQ_ERR 8

void eoi(void);
void init_lapic(void);
void disable_pic(void);

#endif
