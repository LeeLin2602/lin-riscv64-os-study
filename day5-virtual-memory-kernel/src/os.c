#include "printf.h"
#include "mem.h"
#include "time.h"
#include "trap.h"
#include "plic.h"
#include "vm.h"
#include "s_mode.h"

#include "usys.h"

void delay(int count) {
    count *= 50000;
    while (count--) {
        asm volatile ("nop");
    }
}

void kernel_main() {
    printf("Entered S Mode!\n");
    sys_free(sys_malloc(8964 * 2));
    while(1) {
        printf("OS Loop\n");
        delay(10000);
    };
}


void boot() {
    uart_init();
    page_init();
    trap_init();
    timer_init();
    plic_init();
    vm_init();
}

int os_main() {
    boot();
    switch_to_s_mode(kernel_main);
    return 0;
}
