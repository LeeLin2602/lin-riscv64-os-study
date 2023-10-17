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

void test_fs() {
    dump_fs();
    printf("creating directory test1_dir %d\n", fs_mkdir("/", "test1_dir"));
    printf("creating directory test2_dir %d\n", fs_mkdir("/", "test2_dir"));
    printf("creating directory test1_dir/test3_dir %d\n", fs_mkdir("/test1_dir", "test3_dir"));
    dump_fs();
    printf("creating file test1 %d\n", fs_create("/test2_dir", "test1"));
    printf("creating file test2 %d\n", fs_create("/test2_dir", "test2"));
    printf("creating file test3 %d\n", fs_create("/test2_dir/", "test3"));

    char data[] = "Hello World!";
    printf("writing to test2_dir/test2: ");
    printf("%d\n", fs_write("/test2_dir/test2", data, sizeof(data)));

    char res[2048];
    memset(res, 0, 2048);
    printf("READ RESULT %d; ", fs_read("/test2_dir/test2", res, 2048));
    printf("DATA: %s\n", res);

    dump_fs();
    printf("remove file test2 %d\n", fs_rm("/test2_dir/test2"));
    dump_fs();
    printf("remove directory test3_dir %d\n", fs_rmdir("/test1_dir/test3_dir/"));
    dump_fs();
    printf("remove a non-existing file %d\n", fs_rm("/test2_dir/test4"));
    printf("remove a non-existing directory %d\n", fs_rmdir("/test3_dir/"));
    printf("remove all files and directories\n");
    fs_rm("/test2_dir/test1");
    fs_rm("/test2_dir/test3");
    fs_rmdir("/test1_dir");
    fs_rmdir("/test2_dir");
    fs_rmdir("/test3_dir");
    dump_fs();
}

void kernel_main() {
    printf("Entered S Mode!\n");
    test_fs();
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
    if(!fs_init()) {
        printf("build up filesystem.\n");
    }
}

int os_main() {
    boot();
    switch_to_s_mode(kernel_main);
    return 0;
}
