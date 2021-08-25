#include "types.h"
#include "asm.h"
#include "io.h"
#include "uart.h"

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

#define UART_STAT_REG_READY (1 << 0)
#define UART_STAT_REG_EMPTY (1 << 5)

void init_uart(void) {
    outb(COM1 + UART_LCR, 0x80); // DLAB high
    outb(COM1 + UART_BAUD_LOW, 115200 / 9600); // set baud rate to 9600
    outb(COM1 + UART_BAUD_HIGH, 0);
    outb(COM1 + UART_LCR, 0x03); // 8 bits, no parity, one stop big, DLAB low
    outb(COM1 + UART_MODEM_CONTROL, 0x0); // no idea
    outb(COM1 + UART_INT_ENABLE, 0x1); // Enable interrupt
    // outb(COM1 + UART_INT_ENABLE, 0x0); // Enable interrupt
}


void uart_write_byte(uint8_t ch) {
    while (!(inb(COM1 + UART_LINE_STAT) & UART_STAT_REG_EMPTY))
        ;
    outb(COM1 + UART_DATA, ch);
}


int uart_read_byte(void) {
    int ret;
    if (!(inb(COM1 + UART_LINE_STAT) & UART_STAT_REG_READY))
        return -1;
    ret = inb(COM1 + 0);
    if (ret == '\r') {
        ret = '\n';
    }
    return ret;
}


int handle_uartintr(void) {
    int ret = uart_read_byte();
    // echo back input from keyboard
    // so it will be shown on the console
    if (ret == DEL) {
        uart_write_byte('\b');
        uart_write_byte(' ');
        uart_write_byte('\b');
    } else {
        uart_write_byte(ret);
    }
    return ret;
}
