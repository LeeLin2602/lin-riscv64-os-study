#pragma once
#include "riscv.h"

#define BLOCK_SIZE 1024

struct buf {
  int valid;   // has data been read from disk?
  int disk;    // does disk "own" buf?
  int successful; // is the reading successful
  uint32_t dev;
  uint32_t blockno;
  lock_t lock;
  uint32_t refcnt;
  struct buf *prev; // LRU cache list
  struct buf *next;
  unsigned char data[1024];
};
