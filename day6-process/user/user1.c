#include "uart.h"
#include "usys.h"

void _main() {
    while(1){
        putchar('a');
        putchar('\n');
        int cnt = 50000000;
        while(cnt--);
    }
}

