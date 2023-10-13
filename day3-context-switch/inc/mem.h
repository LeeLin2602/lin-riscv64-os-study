#define PAGE_SIZE 4096
#include "printf.h"
#include <stddef.h>
#include <stdint.h>

extern uint32_t TEXT_START;
extern uint32_t TEXT_END;
extern uint32_t DATA_START;
extern uint32_t DATA_END;
extern uint32_t BSS_START;
extern uint32_t BSS_END;
extern void    *HEAP_START;
extern uint32_t HEAP_SIZE;

struct Page {
    int8_t flag;
    void* ptr;
};

void page_init();
void printMEM(int x);
void* malloc(size_t size);
int free(void *ptr);
