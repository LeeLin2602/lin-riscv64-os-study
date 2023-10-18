#pragma once

#define PAGE_SIZE 4096
#include "riscv.h"
#include "lock.h"
#include <stddef.h>

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

void _clear(void *ptr, size_t size);
void page_init();
void printMEM(int x);
void* malloc(size_t size);
void free(void *ptr);
