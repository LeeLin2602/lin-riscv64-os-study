#define riscvos

#include "printf.h"
#include "mem.h"
#include "pmp.h"
#include "time.h"
#include "trap.h"
#include "plic.h"
#include "vm.h"
#include "virtio.h"
#include "bio.h"
#include "s_mode.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "usys.h"

#define BSIZE 512 

void delay(int count) {
    count *= 50000;
    while (count--) {
        asm volatile ("nop");
    }
}

void kernel_main() {
    /* if(!fs_init()) { */
    /*     printf("Build up filesystem.\n"); */
    /* } */
    printf("Enter S Mode.\n");

    struct proc* test1_proc = create_proc();
    struct proc* test2_proc = create_proc();
    exec(test1_proc, "/user1");
    exec(test2_proc, "/user2");
    proc_exec(os_proc, test1_proc);
    
    while(1) {
        printf("OS Loop\n");
        delay(10000);
    };
}

void boot() {
    enable_full_access_all_pmp(); 
    page_init();
    uart_init();
    timer_init();
    trap_init();
    plic_init();
    vm_init();
    virtio_disk_init();
    proc_init();

    // init system process
    os_proc = create_proc();
    set_cpu_proc(os_proc);
}

int os_main(int cpu, uint64_t mem_end) {
    boot();
    switch_to_s_mode(kernel_main);
    /* kernel_main(); */
    return 0;
}
