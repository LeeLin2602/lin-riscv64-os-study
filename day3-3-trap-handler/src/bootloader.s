# .equ REGBYTES, 8
# .equ STACK_SIZE,  ((1 << 12) - 128) 

.section .text.start

.globl _start
_start:
    csrr   t0, mhartid
    lui    t1, 0
    bne    t0, t1, park
  
    la sp, stack_top
    call os_main

park:  
    wfi
    j      park

