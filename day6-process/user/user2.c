#include "printf.h"
#include "usys.h"

int main() { 
    int x = 0;
    for(int i = 0; i <= 10; i++) x += i;
    return x;
}

void _main() {
    int x = main();
    /* x += 1; */
    /* while(1) printf("user process\n"); */
    while(1){
        putchar('b');
    }
    sys_exit(x);
}

