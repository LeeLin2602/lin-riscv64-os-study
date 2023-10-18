#include "timer.h"
#include <stdint.h>

reg_t timer_scratch[1024][5];

// cycles; about 1 second in qemu.
#define interval 10000000 

void timer_init() {
    // each CPU has a separate source of timer interrupts.
    int id = r_mhartid();

    // ask the CLINT for a timer interrupt.
    // int interval = 1000000; // cycles; about 1/10th second in qemu.

    *(reg_t *)(uintptr_t)CLINT_MTIMECMP(id) = *(reg_t *)(uintptr_t)CLINT_MTIME + interval;

    // prepare information in scratch[] for timervec.
    // scratch[0..2] : space for timervec to save registers.
    // scratch[3] : address of CLINT MTIMECMP register.
    // scratch[4] : desired interval (in cycles) between timer interrupts.
    reg_t *scratch = &timer_scratch[id][0];
    scratch[3] = CLINT_MTIMECMP(id);
    scratch[4] = interval;
    w_mscratch((reg_t)scratch);
}

static int timer_count = 0;

void timer_handler() {
    printf("timer_handler: %d\n", ++timer_count);
    int id = r_mhartid();
    *(reg_t *)(uintptr_t)CLINT_MTIMECMP(id) = *(reg_t *)(uintptr_t)CLINT_MTIME + interval;

}
