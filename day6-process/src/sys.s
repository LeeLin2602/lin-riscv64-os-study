# ============ MACRO ==================
.macro ctx_save base
        sd ra, 0(\base)
        sd sp, 8(\base)
        sd s0, 56(\base)
        sd s1, 64(\base)
        sd s2, 136(\base)
        sd s3, 144(\base)
        sd s4, 152(\base)
        sd s5, 160(\base)
        sd s6, 168(\base)
        sd s7, 176(\base)
        sd s8, 184(\base)
        sd s9, 192(\base)
        sd s10, 200(\base)
        sd s11, 208(\base)
        csrr t0, satp     # Read satp into temporary register t0
        sd t0, 248(\base) # Save satp
.endm

.macro ctx_load base

        ld t0, 248(\base) # Load satp into temporary register t0
        sfence.vma zero, zero
        csrw satp, t0     # Write satp from temporary register t0
        sfence.vma zero, zero

        ld ra, 0(\base)
        ld sp, 8(\base)
        ld s0, 56(\base)
        ld s1, 64(\base)
        ld a0, 72(\base)
        ld s2, 136(\base)
        ld s3, 144(\base)
        ld s4, 152(\base)
        ld s5, 160(\base)
        ld s6, 168(\base)
        ld s7, 176(\base)
        ld s8, 184(\base)
        ld s9, 192(\base)
        ld s10, 200(\base)
        ld s11, 208(\base)
.endm

.macro reg_save base
        # save the registers.
        sd ra, 0(\base)
        sd sp, 8(\base)
        sd gp, 16(\base)
        sd tp, 24(\base)
        sd t0, 32(\base)
        sd t1, 40(\base)
        sd t2, 48(\base)
        sd s0, 56(\base)
        sd s1, 64(\base)
        sd a0, 72(\base)
        sd a1, 80(\base)
        sd a2, 88(\base)
        sd a3, 96(\base)
        sd a4, 104(\base)
        sd a5, 112(\base)
        sd a6, 120(\base)
        sd a7, 128(\base)
        sd s2, 136(\base)
        sd s3, 144(\base)
        sd s4, 152(\base)
        sd s5, 160(\base)
        sd s6, 168(\base)
        sd s7, 176(\base)
        sd s8, 184(\base)
        sd s9, 192(\base)
        sd s10, 200(\base)
        sd s11, 208(\base)
        sd t3, 216(\base)
        sd t4, 224(\base)
        sd t5, 232(\base)
        sd t6, 240(\base)
        csrr t0, satp     # Read satp into temporary register t0
        sd t0, 248(\base) # Save satp
.endm

.macro reg_load base
        ld t0, 248(\base) # Load satp into temporary register t0
        csrw satp, t0     # Write satp from temporary register t0
        # restore registers.
        ld ra, 0(\base)
        ld sp, 8(\base)
        ld gp, 16(\base)
        # not this, in case we moved CPUs: ld tp, 24(\base)
        ld t0, 32(\base)
        ld t1, 40(\base)
        ld t2, 48(\base)
        ld s0, 56(\base)
        ld s1, 64(\base)
        ld a0, 72(\base)
        ld a1, 80(\base)
        ld a2, 88(\base)
        ld a3, 96(\base)
        ld a4, 104(\base)
        ld a5, 112(\base)
        ld a6, 120(\base)
        ld a7, 128(\base)
        ld s2, 136(\base)
        ld s3, 144(\base)
        ld s4, 152(\base)
        ld s5, 160(\base)
        ld s6, 168(\base)
        ld s7, 176(\base)
        ld s8, 184(\base)
        ld s9, 192(\base)
        ld s10, 200(\base)
        ld s11, 208(\base)
        ld t3, 216(\base)
        ld t4, 224(\base)
        ld t5, 232(\base)
        ld t6, 240(\base)
.endm

# ============ Macro END   ==================
 
# Context switch
#
#   void sys_switch(struct context *old, struct context *new);
# 
# Save current registers in old. Load from new.

.globl sys_switch
.align 4
sys_switch:
        ctx_save a0  # a0 => struct context *old
        ctx_load a1  # a1 => struct context *new
        ret 


.global trap_vector_s
.align 4
trap_vector_s:

    # Save the context

    addi sp, sp, -256  
    reg_save sp

    li a0, 4321
    call print_int

    # call the C timer_handler(reg_t epc, reg_t cause)

    csrr    a0, mepc
    csrr    a1, mcause
    mv      a2, sp
    call    trap_handler

    # timer_handler will return the return address via a0.
    csrw    mepc, a0

    # Restore the context

    reg_load sp
    addi sp, sp, 256

    mret # back to interrupt location (pc=mepc)

.global trap_vector
.align 4
trap_vector:

    # Save the context

    addi sp, sp, -256  
    reg_save sp

    /* li a0, 1234 */
    /* call print_int */

    # call the C timer_handler(reg_t epc, reg_t cause)

    csrr    a0, mepc
    csrr    a1, mcause
    mv      a2, sp
    call    trap_handler

    # timer_handler will return the return address via a0.
    csrw    mepc, a0

    # Restore the context

    reg_load sp
    addi sp, sp, 256

    mret # back to interrupt location (pc=mepc)



.global atomic_swap
.align 4
atomic_swap:
    li a5, 1
    amoswap.w.aq a5, a5, 0(a0)
    mv a0, a5
    ret
