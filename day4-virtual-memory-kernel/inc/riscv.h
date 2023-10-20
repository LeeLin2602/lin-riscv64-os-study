#pragma once

#include <stdint.h>

#define reg_t uint64_t
#define PAGE_SIZE 4096

static inline reg_t r_mie() {
  reg_t x;
  asm volatile("csrr %0, mie"
               : "=r"(x));
  return x;
}

static inline void w_mie(reg_t x) {
  asm volatile("csrw mie, %0"
               :
               : "r"(x));
}

static inline reg_t r_mstatus() {
  reg_t x;
  asm volatile("csrr %0, mstatus"
               : "=r"(x));
  return x;
}

static inline void w_mstatus(reg_t x) {
  asm volatile("csrw mstatus, %0"
               :
               : "r"(x));
}

static inline reg_t r_sie() {
  reg_t x;
  asm volatile("csrr %0, sie"
               : "=r"(x));
  return x;
}

static inline void w_sie(reg_t x) {
  asm volatile("csrw sie, %0"
               :
               : "r"(x));
}

static inline reg_t r_sstatus() {
  reg_t x;
  asm volatile("csrr %0, sstatus"
               : "=r"(x));
  return x;
}

static inline void w_sstatus(reg_t x) {
  asm volatile("csrw sstatus, %0"
               :
               : "r"(x));
}

static inline reg_t r_tp()
{
  reg_t x;
  asm volatile("mv %0, tp"
               : "=r"(x));
  return x;
}
