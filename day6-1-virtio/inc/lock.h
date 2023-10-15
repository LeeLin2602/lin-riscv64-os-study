#pragma once

typedef struct lock
{
  volatile int locked;
} lock_t;

extern int atomic_swap(lock_t *);
void lock_init(lock_t *lock);
void lock_acquire(lock_t *lock);
void lock_free(lock_t *lock);

