#include "mem.h"
#include "sys.h"
#include "printf.h"
#include "proc.h"

void do_syscall(struct context *ctx) {
    uint32_t syscall_num = ctx->a7;
    /* printf("syscall_num: %d\n", syscall_num); */
    switch (syscall_num) {
    case 1:
        /* printf("parameter = %d\n", ctx -> a0); */
        ctx->a0 = (reg_t)malloc(ctx->a0);
        break;
    case 2:
        free((void *)ctx->a0);
        break;
    case 3:
        printf("program has exitted with code %d on core %d.\n", ctx->a0, r_tp());
        proc_exec(cpus[r_mhartid()].proc, os_proc);
        /* while(1); */
        break;
    default:
        printf("Unknown syscall no: %d\n", syscall_num);
        ctx->a0 = -1;
    }
}
