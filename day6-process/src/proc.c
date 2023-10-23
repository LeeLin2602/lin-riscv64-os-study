#include "proc.h"
#include "printf.h"
#include "string.h"
#include "fs.h"
#include "mem.h"
#include "vm.h"
#include "riscv.h"

#define PROGRAM_SIZE 1024 * 1024

int nextpid = 1;
lock_t pid_lock;
lock_t sched_lock;
struct cpu cpus[NCPU];
struct proc processes[NPROC];
struct proc* os_proc;

int allocpid() {
    int pid;

    lock_acquire(&pid_lock);
    pid = nextpid;
    nextpid = nextpid + 1;
    lock_free(&pid_lock);

    return pid;
}

void proc_init() {
    lock_init(&pid_lock);
    lock_init(&sched_lock);
	struct proc *p;
	for(p = processes; p < &processes[NPROC]; p++) {
		lock_init(&p->lock);
		p->state = UNUSED;
		p->stack = malloc(STACK_SIZE);
        p->page_table = malloc(PAGE_SIZE);
	}
}

struct proc* create_proc() {
	struct proc *p;
	for(p = processes; p < &processes[NPROC]; p++) {
		lock_acquire(&p->lock);
        if(p->state == UNUSED) {
            p->state = USED;
            p->pid = allocpid();
            struct context *ctx = malloc(sizeof(struct context));
            ctx->ra = (reg_t) 0;
            ctx->sp = (reg_t) p->stack + STACK_SIZE - 1;
            ctx->satp = MAKE_SATP(p->page_table);
            p->context = ctx;

            lock_free(&p->lock);
            map_pages(p->page_table, (uint64_t)p->stack,(uint64_t)p->stack, STACK_SIZE, PTE_R | PTE_W);
            map_pages(p->page_table, (uint64_t)p->context, (uint64_t)p->context, PAGE_SIZE, PTE_R | PTE_W);
            // here might be a security issue that userspace programs can access part of kernel code.
            map_pages(p->page_table, (uint64_t)sys_switch, (uint64_t)sys_switch, PAGE_SIZE, PTE_R | PTE_X);
            return p;
        }
        lock_free(&p->lock);
	}
    return NULL;
}

uint64_t getELFEntryPoint(const char* file) {
    struct elfhdr ehdr;
    memcpy(&ehdr, file, sizeof(ehdr));

    if(ehdr.magic != ELF_MAGIC) {
        printf("Not an ELF file\n");
        return -1;
    }

	return ehdr.entry + 0x1000;
}

int exec(struct proc* process, const char* filename) {
	lock_acquire(&process->lock);
    char * elf = malloc(PROGRAM_SIZE);
    fs_read(filename, elf, PROGRAM_SIZE);

	uint64_t entryPoint = getELFEntryPoint(elf);
	if(entryPoint == -1) return 1;

    process->context->ra = (reg_t)elf + entryPoint;
    process->instrs = elf;
    process->state = RUNNABLE;

    map_pages(process->page_table, (uint64_t)elf, (uint64_t)elf, PROGRAM_SIZE, PTE_R | PTE_X);
	map_pages(process->page_table, UART, UART, PAGE_SIZE, PTE_R | PTE_W);

    lock_free(&process->lock);
	return 0;
}

void set_cpu_proc(struct proc* next) {
    cpus[r_tp()].proc = next;
}

void proc_exec(struct proc* cur, struct proc* next) {
    set_cpu_proc(next);
    cur->state = RUNNABLE;
    sys_switch(cur->context, next->context);
    trap_init();
}

void scheduler() {
	struct proc *p;
	for(p = processes; p < &processes[NPROC]; p++) {
        lock_acquire(&sched_lock);
		lock_acquire(&p->lock);
        if(p->state == RUNNABLE) {
            p->state = RUNNING;
            proc_exec(cpus[r_tp()].proc, p);
        }
        lock_free(&p->lock);
        lock_free(&sched_lock);
    }
}
