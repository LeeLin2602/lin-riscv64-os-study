static inline void w_pmpaddr0(uint64_t x) {
    asm volatile("csrw pmpaddr0, %0" : : "r" (x));
}

static inline void w_pmpcfg0(uint64_t x) {
    asm volatile("csrw pmpcfg0, %0" : : "r" (x));
}

static inline void w_pmpaddr2(uint64_t x) {
    asm volatile("csrw pmpaddr2, %0" : : "r" (x));
}

static inline void w_pmpcfg2(uint64_t x) {
    asm volatile("csrw pmpcfg2, %0" : : "r" (x));
}

void enable_full_access_all_pmp() {
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);
    w_pmpaddr2(0x3fffffffffffffull);
    w_pmpcfg2(0xf);
}
