.global sys_malloc
sys_malloc:
    li a7, 1
    ecall
    ret

.global sys_free
sys_free:
    li a7, 2
    ecall
    ret
