#include "vm.h"
#include "string.h"

#define KERNEL_START_ADDR   0x80000000ll
#define KERNEL_END_ADDR     0x88000000ll
#define VIRTUAL_BASE        0x80000000ll

pagetable_t root_pt;

pte_t* walk(pagetable_t pagetable, uint64_t va) {
    if(va >= MAXVA)
        return 0;
    for(int level = 2; level > 0; level--) {
        pte_t *pte = &pagetable[PX(level, va)];
        if(*pte & PTE_V) {
            pagetable = (pagetable_t)PTE2PA(*pte);
        } else {
            if((pagetable = (pde_t*)malloc(4096)) == 0)
                return 0;
            memset(pagetable, 0, PAGE_SIZE);
            *pte = PA2PTE(pagetable) | PTE_V;
        }
    }
    return &pagetable[PX(0, va)];
}

int map_page(pagetable_t pagetable, uint64_t va, uint64_t pa, int perm) {
 	va = PGROUNDDOWN(va);
 	pa = PGROUNDDOWN(pa);

	pte_t *pte = walk(pagetable, va);
	
	if(pte == 0) 
		return -1;
    if(*pte & PTE_V)
     	return -2;

    *pte = PA2PTE(pa) | perm | PTE_V;
	return 0;
}

int map_pages(pagetable_t pagetable, uint64_t va, uint64_t pa, uint64_t size, int perm) {
	uint64_t a, last;

	if(size == 0)
	return 0;

	a = PGROUNDDOWN(va);
	last = PGROUNDDOWN(va + size - 1);

	for(;a <= last; a += PAGE_SIZE, pa += PAGE_SIZE)
		map_page(pagetable, a, pa, perm);

	return 0;
}

// Make a direct-map page table for the kernel.
void kernel_mapping(pagetable_t pt) {
	// uart registers
	map_page(pt, UART, UART, PTE_R | PTE_W);
	// virtio mmio disk interface
	map_page(pt, VIRTIO0, VIRTIO0, PTE_R | PTE_W);
	// PLIC
	map_pages(pt, PLIC_BASE, PLIC_BASE, 0x400000, PTE_R | PTE_W);
	// map kernel
	map_pages(pt, KERNEL_START_ADDR, VIRTUAL_BASE, KERNEL_END_ADDR - KERNEL_START_ADDR, PTE_W | PTE_R | PTE_X);
}

void kvm_init(void) {
 	root_pt = malloc(PAGE_SIZE);
	memset(root_pt, 0, PAGE_SIZE);
 	kernel_mapping(root_pt);
}

void vm_init() {
    kvm_init();
    // wait for any previous writes to the page table memory to finish.
    sfence_vma();
    w_satp(MAKE_SATP(root_pt));
    sfence_vma(); // flush stale entries from the TLB.
}

