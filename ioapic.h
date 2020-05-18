#ifndef IOAPIC_H
#define IOAPIC_H

#include "types.h"


void disable_ioapic(uint32_t irq);


void enable_ioapic(uint32_t irq);
void init_ioapic(void);

#endif
