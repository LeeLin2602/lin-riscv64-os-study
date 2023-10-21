#include "timer.h"
#include "proc.h"
#include "param.h"

#include <stdint.h>

reg_t timer_scratch[NPROC][5];

// cycles; about 0.1 second in qemu.
#define interval 1000000

void timer_init() {
    // each CPU has a separate source of timer interrupts.
    int id = r_mhartid();

    // ask the CLINT for a timer interrupt.

    *(reg_t *)(uintptr_t)CLINT_MTIMECMP(id) = *(reg_t *)(uintptr_t)CLINT_MTIME + interval;

    // prepare information in scratch[] for timervec.
    // scratch[0..2] : space for timervec to save registers.
    // scratch[3] : address of CLINT MTIMECMP register.
    // scratch[4] : desired interval (in cycles) between timer interrupts.
    reg_t *scratch = &timer_scratch[id][0];
    scratch[3] = CLINT_MTIMECMP(id);
    scratch[4] = interval;
    w_mscratch((reg_t)scratch);
    w_sscratch((reg_t)scratch);
}

static int timer_count = 0;

void timer_handler() {
    int id = r_mhartid();
    *(reg_t *)(uintptr_t)CLINT_MTIMECMP(id) = *(reg_t *)(uintptr_t)CLINT_MTIME + interval;
    proc_exec(cpus[r_mhartid()].proc, os_proc);
}
