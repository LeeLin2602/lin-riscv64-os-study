#include "uart.h"

#define LSR_RX_READY (1 << 0)
#define EOF 0

void uart_init()
{
    /* disable interrupts */
    UART_REGW(UART_IER, 0x00);

    /* Baud rate setting */
    uint8_t lcr = UART_REGR(UART_LCR);
    UART_REGW(UART_LCR, lcr | (1 << 7));
    UART_REGW(UART_DLL, 0x03);
    UART_REGW(UART_DLM, 0x00);

    lcr = 0;
    UART_REGW(UART_LCR, lcr | (3 << 0));

    uint8_t ier = UART_REGR(UART_IER);
    UART_REGW(UART_IER, ier | (1 << 0));
}

void putchar(char c) {
    *(char *)UART = c;
}

int getchar(void) {
    if (*UART_LSR & LSR_RX_READY)
    {
        return *UART_RHR == '\r' ? '\n' : *UART_RHR;
    }
    else
    {
        return -1;
    }
}
