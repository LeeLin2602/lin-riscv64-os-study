#include "printf.h"
#include "mem.h"
#include "time.h"
#include "trap.h"
#include "plic.h"

#include "usys.h"

void delay(volatile int count) {
    count *= 50000;
    while (count--) {
        asm volatile ("nop");
    }
}

void boot() {
    uart_init();
    page_init();
    trap_init();
    timer_init();
    plic_init();
    printf("OS start\n");
}

int os_main(void) {
    boot();
    
    while(1) {
        printf("OS Loop\n");
        sys_free(sys_malloc(4096 * 3));
        delay(10000);
    };
    return 0;
}
