#include "printf.h"
#include "usys.h"

int _main() { 
    int x = 0;
    for(int i = 0; i <= 10; i++) x += i;
    return x;
}

/* void _main() { */
/*     int x = main(); */
/*     x += 1; */
/*     /1* while(1) printf("user process\n"); *1/ */
/*     /1* sys_exit(x); *1/ */
/* } */

