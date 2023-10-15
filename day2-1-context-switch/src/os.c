#include "printf.h"
#include "mem.h"
#include "sys.h"

#define STACK_SIZE 1024
__attribute__((aligned(16))) reg_t task0_stack[STACK_SIZE];
struct context ctx_os;
struct context ctx_task;


void user_task0(void)
{
    printf("Task0: Context Switch Success !\n");
    sys_switch(&ctx_task, &ctx_os);
}

int os_main(void) {
    page_init();
    printf("OS start\n");
    ctx_task.ra = (reg_t) user_task0;
    ctx_task.sp = (reg_t) &task0_stack[STACK_SIZE - 1];
    printf("ctx_task = 0x%x\n", &ctx_task);
    sys_switch(&ctx_os, &ctx_task);
    printf("OS Loop\n");
    while(1);
    return 0;
}
