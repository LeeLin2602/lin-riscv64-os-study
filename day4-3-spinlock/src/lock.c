#include "lock.h"

void lock_init(lock_t *lock) {
    lock->locked = 0;
}

void lock_acquire(lock_t *lock) {
    for (;;) {
        if (!atomic_swap(lock)) {
           break;
        }
    }
}

void lock_free(lock_t *lock) {
    lock->locked = 0;
}

