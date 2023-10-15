#include "vm.h"

#define KERNEL_START_ADDR 0x80000000
#define KERNEL_END_ADDR   0x80200000
#define VIRTUAL_BASE      0x80000000

pte_t root_pt[PT_ENTRY_COUNT] __attribute__ ((aligned (PAGE_SIZE)));


pte_t* walk(pagetable_t pagetable, uint64_t va) {
    if(va >= MAXVA)
        return 0;

    for(int level = 2; level > 0; level--) {
        pte_t *pte = &pagetable[PX(level, va)];
        if(*pte & PTE_V) {
            pagetable = (pagetable_t)PTE2PA(*pte);
        } else {
            if((pagetable = (pde_t*)malloc(PAGE_SIZE)) == 0)
                return 0;
            _clear(pagetable, PAGE_SIZE);
            *pte = PA2PTE(pagetable) | PTE_V;
        }
    }
    return &pagetable[PX(0, va)];
}

int map_page(pagetable_t pagetable, uint64_t va, uint64_t pa, int perm) {
    pte_t *pte = 0;

    va = PGROUNDDOWN(va);

    pte =  walk(pagetable, va);

    if(pte == 0) 
        return -1;

    if(*pte & PTE_V) {
        printf("%llx %x\n", pte, *pte);
        return -2;
    }

    *pte = PA2PTE(pa) | perm | PTE_V;

  return 0;
}


int map_pages(pagetable_t pagetable, uint64_t va, uint64_t size, uint64_t pa, int perm) {
    uint64_t a, last;
    if(size == 0)
        return -1;

    a = PGROUNDDOWN(va);
    last = PGROUNDDOWN(va + size - 1);
    for(;a <= last;){
        int x = map_page(pagetable, a, pa, perm);
        if(x != 0) {
            printf("mapping from %x to %x occurs error %d.\n", a, pa, x);
        }
        a += PAGE_SIZE;
        pa += PAGE_SIZE;
    }
    return 0;
}


void map_kernel() {
    _clear(root_pt, PAGE_SIZE);
    
    map_page(root_pt, UART, UART, PTE_X | PTE_R | PTE_W);
    map_pages(root_pt, VIRTIO0, PAGE_SIZE, VIRTIO0, PTE_R | PTE_W);
    map_pages(root_pt, PLIC_BASE, 0x400000, PLIC_BASE, PTE_X | PTE_R | PTE_W);
    map_pages(root_pt, KERNEL_START_ADDR, KERNEL_END_ADDR - KERNEL_START_ADDR, VIRTUAL_BASE, PTE_R | PTE_W | PTE_X);
}

void vm_init() {
    map_kernel();
    sfence_vma();
    w_satp(MAKE_SATP(root_pt));
}
