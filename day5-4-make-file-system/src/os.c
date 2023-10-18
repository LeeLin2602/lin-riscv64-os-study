#define riscvos
#include "printf.h"
#include "mem.h"
#include "time.h"
#include "trap.h"
#include "plic.h"
#include "vm.h"
#include "virtio.h"
#include "bio.h"
#include "s_mode.h"
#include "string.h"
#include "fs.h"

#include "usys.h"

#define BSIZE 512 

void delay(int count) {
    count *= 50000;
    while (count--) {
        asm volatile ("nop");
    }
}

void kernel_main() {
    printf("Entered S Mode!\n");
    char buf[2048];
    dump_fs();
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
    virtio_disk_init();
    printf("Init Disk: %d\n", fs_init());
}

int os_main() {
    boot();
    switch_to_s_mode(kernel_main);
    return 0;
}
