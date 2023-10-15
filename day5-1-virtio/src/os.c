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

void test_virtio_disk(void) {
    const int test_blockno = 10;  
    char data[BSIZE];
    struct buf b;

    b.blockno = test_blockno;

    printf("Reading data from VIRTIO.\n");
    virtio_disk_rw(&b, 0); 
    memcpy(data, b.data, BSIZE);
    printf("Original data at block %d: %s\n", test_blockno, data);

    strcpy(b.data, "Hello, Virtio Disk!");
    virtio_disk_rw(&b, 1);

    memset(b.data, 0, BSIZE);
    virtio_disk_rw(&b, 0);
    printf("New data at block %d: %s\n", test_blockno, b.data);

    if(b.successful) {
        printf("Test successful.\n");
    } else {
        printf("Test failed.\n");
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
    test_virtio_disk();
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
