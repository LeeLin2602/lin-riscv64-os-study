#pragma once

#include "lock.h"
#include "string.h"
#include "mem.h"
#include "virtio.h"
#include "ds.h"

int bread(uint32_t dev, uint32_t blockno, void *buffer);
int bwrite(uint32_t dev, uint32_t blockno, void *data);
