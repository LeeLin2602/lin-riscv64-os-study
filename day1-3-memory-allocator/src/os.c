#include "printf.h"
#include "mem.h"

int os_main(void) {
    page_init();
    printf("Hello World\n");
    int* arr1 = malloc(sizeof(int) * 4096);
    int* arr2 = malloc(sizeof(int) * 4096);
    printMEM(10);
    printf("0x%x, 0x%x\n", arr1, arr2);
    free(arr1);
    printMEM(10);
    printf("0x%x 0x%x 0x%x\n", malloc(sizeof(int) * 1024), malloc(sizeof(int) * 1024), malloc(sizeof(int) * 1024));
    printMEM(10);
    while(1);
    return 0;
}
