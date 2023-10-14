#include "mem.h"
#include "sys.h"

void do_syscall(struct context *ctx) {
    uint32_t syscall_num = ctx->a7;
    /* printf("syscall_num: %d\n", syscall_num); */
    switch (syscall_num) {
    case 1:
        /* printf("parameter = %d\n", ctx -> a0); */
        ctx->a0 = (reg_t)malloc(ctx->a0);
        break;
    case 2:
        ctx->a0 = free((void *)ctx->a0);
        break;
    default:
        printf("Unknown syscall no: %d\n", syscall_num);
        ctx->a0 = -1;
    }
}
