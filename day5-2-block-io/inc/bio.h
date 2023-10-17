#include "lock.h"
#include "string.h"
#include "mem.h"
#include "virtio.h"
#include "ds.h"

int bread(uint32_t dev, uint32_t blockno, char * buffer);
int bwrite(uint32_t dev, uint32_t blockno, const char *data);
