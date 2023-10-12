volatile char *uart = (volatile char*) 0x10000000;

void print(const char *str) {
    while (*str) {
        *uart = *(str++);
    }
}

__attribute__ ((aligned (16))) char stack0[8192];

int os_main(void) {
    print("Hello, QEMU!\n");
    while (1) {}
    return 0;
}
