#include "printf.h"
#include "usys.h"

int main() { 
    int x = 0;
    for(int i = 0; i <= 10; i++) x += i;
    return x;
}

void _main() {
    int x = main();
    while(1){
        putchar('b');
        putchar('\n');
        int cnt = 50000000;
        while(cnt--);
    }
    sys_exit(x);
}

