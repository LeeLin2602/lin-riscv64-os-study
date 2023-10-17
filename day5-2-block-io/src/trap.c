#include "trap.h"

void trap_init() {
    // set the machine-mode trap handler.
    w_mtvec((reg_t)trap_vector);
    // enable machine-mode external interrupts.
    w_mie(r_mie() | MIE_MEIE);
    // enable machine-mode timer interrupts.
    w_mie(r_mie() | MIE_MTIE);
    // enable machine-mode interrupts.
    w_mstatus(r_mstatus() | MSTATUS_MIE);
}

void external_handler() {
  int irq = plic_claim();
    if (irq == UART0_IRQ) {
        /* for (;;) { */
        /*     int c = getchar(); */
        /*     if (c == -1) */
        /*     { */
        /*         break; */
        /*     } */
        /*     else */
        /*     { */
        /*         printf("USER INPUT %c\n", (char)c); */
        /*     } */
        /* } */
    }
    else if (irq == VIRTIO_IRQ) {
        virtio_disk_intr();
    } else if (irq) {
        printf("unexpected interrupt irq = %d\n", irq);
    }

    if (irq) {
        plic_complete(irq);
    }
}

reg_t trap_handler(reg_t epc, reg_t cause, struct context *ctx) {
    reg_t return_pc = epc;
    reg_t cause_code = cause & 0xfff;

    /* printf("0x%x", cause); */
    if (cause & 0x8000000000000000LL)
    {
        /* Asynchronous trap - interrupt */
        switch (cause_code) {
            case 3:
                printf("software interruption!\n");
                break;
            case 7:
                // disable machine-mode timer interrupts.
                w_mie(~((~r_mie()) | (1 << 7)));
                timer_handler();
                // enable machine-mode timer interrupts.
                w_mie(r_mie() | MIE_MTIE);
                
                break;
            case 11:
                external_handler();
                break;
            default:
                printf("unknown async exception!\n");
                break;
        }
    } else {
        /* Synchronous trap - exception */
        switch (cause_code) {
            case 2:
                printf("Illegal instruction!\n");
                while(1);
                break;
            case 5:
                printf("Fault load!\n");
                while(1);
                break;
            case 7:
                printf("Fault store!\n");
                while(1);
                break;
            case 8:
                /* printf("Environment call from U-mode!\n"); */
                do_syscall(ctx);
                return_pc += 4;
                break;
            case 9:
                /* printf("Environment call from S-mode!\n"); */
                do_syscall(ctx);
                return_pc += 4;
                break;
            case 11:
                /* printf("Environment call from M-mode!\n"); */
                do_syscall(ctx);
                return_pc += 4;
                break;
            case 12:
                printf("Fault instruction page!\n");
                /* return_pc += 4; */
                while(1);
                break;
            case 13:
                printf("Fault page loading!\n");
                while(1);
            default:
                 /* Synchronous trap - exception */
                printf("Sync exceptions! cause code: %d\n", cause_code);
                break;
        }
        /* return_pc += 2; */
    }
    return return_pc;
}
