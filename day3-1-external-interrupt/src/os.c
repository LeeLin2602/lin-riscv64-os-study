#include "printf.h"
#include "mem.h"
#include "time.h"
#include "trap.h"
#include "plic.h"

void delay(volatile int count) {
    count *= 50000;
    while (count--) {
        asm volatile ("nop");
    }
}

int os_main(void) {
    uart_init();
    page_init();
    trap_init();
    timer_init();
    plic_init();
    printf("OS start\n");
    printf("test %c\n", 'a');
    while(1) {
        printf("OS Loop\n");
        delay(10000);
    };
    return 0;
}
