#pragma once
#include "vm.h"
#include "param.h"
#include "lock.h"
#include "sys.h"
#include "mem.h"

#define STACK_SIZE 8192 
#define TRAMPOLINE (MAXVA - PAGE_SIZE)
#define TRAPFRAME (TRAMPOLINE - PAGE_SIZE)

struct cpu {
	struct proc* proc;          // The process running on this cpu, or null.
};

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE  };

struct proc {
  lock_t lock;

  // p->lock must be held when using these:
  enum procstate state;        // Process state
  int killed;                  // If non-zero, have been killed
  int pid;                     // Process ID

  // wait_lock must be held when using this:
  struct proc *parent;         // Parent process

  // these are private to the process, so p->lock need not be held.
  void* instrs;
  void* stack;                 // Virtual address of kernel stack
  uint64_t sz;                 // Size of process memory (bytes)
  pte_t* page_table;           // User page table
  struct context *context;     // sys_switch() here to run process
  char name[16];               // Process name (debugging)
};

int allocpid();
void proc_init();
struct proc* create_proc();
int exec(struct proc* process, const char* filename);
void set_cpu_proc(struct proc* next);
void proc_exec(struct proc* cur, struct proc* nxt);
void scheduler();

extern struct cpu cpus[NCPU];
extern struct proc* os_proc;


/* ELF define */
#define ELF_MAGIC 0x464C457FU  // "\x7FELF" in little endian

// File header
struct elfhdr {
  uint32_t magic;  // must equal ELF_MAGIC
  uint8_t elf[12];
  uint16_t type;
  uint16_t machine;
  uint32_t version;
  uint64_t entry;
  uint64_t phoff;
  uint64_t shoff;
  uint32_t flags;
  uint16_t ehsize;
  uint16_t phentsize;
  uint16_t phnum;
  uint16_t shentsize;
  uint16_t shnum;
  uint16_t shstrndx;
};

