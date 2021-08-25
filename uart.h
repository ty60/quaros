#ifndef UART_H
#define UART_H

#include "types.h"

#define IRQ_SERIAL 4

#define DEL 0x7f


void init_uart(void);
void uart_write_byte(uint8_t ch);
int handle_uartintr(void);

#endif
