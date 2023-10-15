#include "mem.h"

static void *_alloc_start = 0;
static void *_alloc_end = 0;
static uint32_t _num_pages = 0;

void _clear(void* ptr, size_t size) {
    unsigned char* byte_ptr = (unsigned char*)ptr;
    for(size_t i = 0; i < size; i++) {
        byte_ptr[i] = 0;
    }
}

void page_init() {
    _num_pages = HEAP_SIZE / (PAGE_SIZE + sizeof(struct Page));
    printf("HEAP_START =0x%x, HEAP_SIZE = 0x%x, num of pages = %d\n", HEAP_START, HEAP_SIZE, _num_pages);

    _alloc_start = HEAP_START + _num_pages * sizeof(struct Page);
    _alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);

    struct Page *page = (struct Page *)HEAP_START;
    /* printf("%x\n", page -> flag); */
    for (uint32_t i = 0; i < _num_pages; i++)
    {
        /* printf("%x\n", page->flag); */
        page->flag = 0;
        page->ptr = _alloc_start + i * PAGE_SIZE;
        page++;   
    }

    printf("HEAP:   0x%x -> 0x%x\n", _alloc_start, _alloc_end);
}

void printMEM(int x) {
    struct Page *page = (struct Page *)HEAP_START;
    for(int i = 0; i < x; i++, page++) {
        printf("| %d ", page -> flag);
    }
    printf("|\n");
}

void* malloc(size_t size) {
    int N = (size + PAGE_SIZE - 1) / PAGE_SIZE; // ceil
    struct Page *page = (struct Page *)HEAP_START;
    for(int i = 0; i < _num_pages; i++, page++) {
        struct Page *start_pos = page; 
        int j;
        for(j = 0; j < N; j++, i++, page++) {
            if(page->flag != 0) {
                break;
            }
        }
        if(j == N) {
            page = start_pos;
            for(int j = 0; j < N; j++, page++) {
                page->flag = 1;
            }
            page--; page->flag = 2;
            return start_pos->ptr;
        }
    }
    return 0x0;
}

int free(void* ptr) {
    struct Page *page = (struct Page *)HEAP_START;
    for(int i = 0; i < _num_pages; i++, page++) {
        if(page->ptr == ptr) break;
        if(page->ptr > ptr) return 1;
    }
    while(page->flag == 1) {
        _clear(page->ptr, PAGE_SIZE);
        page->flag = 0;
        page++;
    }
    _clear(page->ptr, PAGE_SIZE);
    page->flag = 0;
    return 0;
}
