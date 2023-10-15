#include "riscv.h"

#define UART 0x10000000L
#define UART_THR (volatile uint8_t *)(UART + 0x00) // THR:transmitter holding register
#define UART_RHR (volatile uint8_t *)(UART + 0x00) // RHR:Receive holding register
#define UART_DLL (volatile uint8_t *)(UART + 0x00) // LSB of Divisor Latch (write mode)
#define UART_DLM (volatile uint8_t *)(UART + 0x01) // MSB of Divisor Latch (write mode)
#define UART_IER (volatile uint8_t *)(UART + 0x01) // Interrupt Enable Register
#define UART_LCR (volatile uint8_t *)(UART + 0x03) // Line Control Register
#define UART_LSR (volatile uint8_t *)(UART + 0x05) // LSR:line status register
#define UART_LSR_EMPTY_MASK 0x40                   // LSR Bit 6: Transmitter empty; both the THR and LSR are empty

#define UART_REGR(reg) (*(reg))
#define UART_REGW(reg, v) ((*reg) = (v))

#define UART0_IRQ 10
#define VIRTIO_IRQ 1

void putchar(char c);
int getchar(void);
