#pragma once
#include "riscv.h"

int bread(uint32_t dev, uint32_t blockno, void *buffer);
int bwrite(uint32_t dev, uint32_t blockno, void *data);
