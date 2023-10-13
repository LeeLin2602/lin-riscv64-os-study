#include "printf.h"
#include "mem.h"
#include "time.h"
#include "trap.h"

void delay(volatile int count) {
    count *= 50000;
    while (count--) {
        asm volatile ("nop");
    }
}

int os_main(void) {
    page_init();
    trap_init();
    timer_init();
    printf("OS start\n");
    while(1) {
        printf("OS Loop\n");
        delay(1000);
    };
    return 0;
}
