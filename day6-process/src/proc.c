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

            /* memset(p->stack, 0, STACK_SIZE); */
            lock_free(&p->lock);
            printf("%llx\n", sys_switch);
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

void test() {
    /* printf("hello, Test.\n"); */
    /* sys_exit(0); */
    /* sys_free(sys_malloc(4096)); */
}

void exec(struct proc* process, const char* filename) {
    char * instrs = malloc(PROGRAM_SIZE);
    size_t size = fs_read(filename, instrs, PROGRAM_SIZE);
    /* process->context.ra = test; //instrs + 0x1042; */
    process->context->ra = instrs + 0x1042;
    map_pages(process->page_table, (uint64_t)instrs, (uint64_t)instrs, ((size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE, PTE_R | PTE_X);
	map_page(process->page_table, UART, UART, PTE_R | PTE_W);
}

void set_cpu_proc(struct proc* next) {
    cpus[r_tp()].proc = next;
}
void proc_exec(struct proc* cur, struct proc* next) {
    set_cpu_proc(next);
    sys_switch(cur->context, next->context);
}
