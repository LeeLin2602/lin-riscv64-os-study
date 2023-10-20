static inline void write_mstatus(uint64_t value) {
    asm volatile("csrw mstatus, %0" : : "r"(value));
}

static inline void write_mepc(uint64_t value) {
    asm volatile("csrw mepc, %0" : : "r"(value));
}

static inline uint64_t read_mstatus() {
    uint64_t value;
    asm volatile("csrr %0, mstatus" : "=r"(value));
    return value;
}

void switch_to_s_mode(void (*s_mode_entry)) {
    // Set the mepc register to the S-mode entry point
    write_mepc((uint64_t)s_mode_entry);

    // Set the MPP field in the mstatus register to S-mode
    uint64_t mstatus = read_mstatus();
    mstatus &= ~MSTATUS_MPP_MASK;
    mstatus |= MSTATUS_MPP_S;
    write_mstatus(mstatus);

    // Execute the mret instruction to switch to S-mode
    asm volatile("mret");
}

