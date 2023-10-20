#include "sys.h"
#include "riscv.h"
#include "printf.h"

#define CLINT 0x2000000
#define CLINT_MTIMECMP(hartid) (CLINT + 0x4000 + 4*(hartid))
#define CLINT_MTIME (CLINT + 0xBFF8) // cycles since boot.

// Machine Scratch register, for early trap handler
static inline void w_mscratch(reg_t x)
{
  asm volatile("csrw mscratch, %0" : : "r" (x));
}

// Supervisor Scratch register, for early trap handler
static inline void w_sscratch(reg_t x)
{
  asm volatile("csrw sscratch, %0" : : "r" (x));
}

void timer_init();
void timer_handler();

