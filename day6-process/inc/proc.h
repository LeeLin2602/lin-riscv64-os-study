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
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID

  // wait_lock must be held when using this:
  struct proc *parent;         // Parent process

  // these are private to the process, so p->lock need not be held.
  void* stack;                 // Virtual address of kernel stack
  uint64_t sz;                 // Size of process memory (bytes)
  pte_t* page_table;           // User page table
  struct trapframe *trapframe; // data page for trampoline.S
  struct context *context;      // sys_switch() here to run process
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
};

int allocpid();
void proc_init();
struct proc* create_proc();
void exec(struct proc* process, const char* filename);
void proc_exec(struct proc* cur, struct proc* nxt);

extern struct cpu cpus[NCPU];
extern struct proc* os_proc;
