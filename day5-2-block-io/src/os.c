#include "printf.h"
#include "mem.h"
#include "time.h"
#include "trap.h"
#include "plic.h"
#include "vm.h"
#include "virtio.h"
#include "s_mode.h"
#include "string.h"

#include "usys.h"

#define BSIZE 512 

void test_bio(void) {
    const int test_blockno = 1;  
    
    printf("Writing \"Hello World!\" to disk.\n");

    char data[1024] = "Hello World!";

    int w_status = bwrite(0, 1, data);

    if(w_status) {
        printf("writing successful.\n");
    }
    
    memset(data, 0, BLOCK_SIZE);
    int r_status = bread(0, 1, &data[0]);
    
    if(r_status) {
        printf("reading successful.\n");
        printf("Read data: %s\n", data);
    }

}

void delay(int count) {
    count *= 50000;
    while (count--) {
        asm volatile ("nop");
    }
}

void kernel_main() {
    printf("Entered S Mode!\n");
    test_bio();
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
}

int os_main() {
    boot();
    switch_to_s_mode(kernel_main);
    return 0;
}
