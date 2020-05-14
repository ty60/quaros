#include "types.h"
#include "asm.h"

#define COM1 0x3f8

#define UART_DATA 0x0
#define UART_BAUD_LOW 0x0
#define UART_INT_ENABLE 0x1
#define UART_BAUD_HIGH 0x1
#define UART_FIFO 0x2
#define UART_LCR 0x3
#define UART_MODEM_CONTROL 0x4
#define UART_LINE_STAT 0x5
#define UART_MODEM_STAT 0x6
#define UART_SCRATCH 0x7


void init_uart(void) {
    outb(COM1 + UART_LCR, 0x80); // DLAB high
    outb(COM1 + UART_BAUD_LOW, 115200 / 9600); // set baud rate to 9600
    outb(COM1 + UART_BAUD_HIGH, 0);
    outb(COM1 + UART_LCR, 0x03); // 8 bits, no parity, one stop big, DLAB low
    outb(COM1 + UART_MODEM_CONTROL, 0x0); // no idea
    outb(COM1 + UART_INT_ENABLE, 0x0); // Disable interrupt
}


void uart_write_byte(uint8_t ch) {
    while (!(inb(COM1 + UART_LINE_STAT) & 0x20))
        ;
    outb(COM1 + UART_DATA, ch);
}
