#ifndef UART_H
#define UART_H

#include "types.h"

void init_uart(void);
void uart_write_byte(uint8_t ch);

#endif
