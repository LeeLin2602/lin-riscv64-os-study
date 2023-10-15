#include "mem.h"

static void *_alloc_start = 0;
static void *_alloc_end = 0;
static uint32_t _num_pages = 0;

lock_t lock;

void _clear(void* ptr, size_t size) {
    unsigned char* byte_ptr = (unsigned char*)ptr;
    for(size_t i = 0; i < size; i++) {
        byte_ptr[i] = 0;
    }
}

void page_init() {
    _num_pages = HEAP_SIZE / (PAGE_SIZE + sizeof(struct Page)) - 1; // -1 for align

    _alloc_start = (void*)((((uint64_t)HEAP_START + _num_pages * sizeof(struct Page) + 4095) / 4096) * 4096);
    _alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);

    struct Page *page = (struct Page *)HEAP_START;
    for (uint32_t i = 0; i < _num_pages; i++)
    {
        page->flag = 0;
        page->ptr = _alloc_start + i * PAGE_SIZE;
        page++;   
    }

    lock_init(&lock);
}

void printMEM(int x) {
    lock_acquire(&lock);
    struct Page *page = (struct Page *)HEAP_START;
    for(int i = 0; i < x; i++, page++) {
        printf("| %d ", page -> flag);
    }
    lock_free(&lock);
    printf("|\n");
}

void* malloc(size_t size) {
    lock_acquire(&lock);
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
            lock_free(&lock);
            return start_pos->ptr;
        }
    }
    lock_free(&lock);
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
