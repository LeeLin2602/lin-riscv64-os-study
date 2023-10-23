#include "uart.h"
#include "usys.h"

int a = 0;

void _main() {
    while(1){
        putchar('a');
        putchar('\n');
        a = 50000000;
        while(a--);
    }
}

