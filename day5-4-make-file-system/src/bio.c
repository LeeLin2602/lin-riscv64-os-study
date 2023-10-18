#include "bio.h"
#include "lock.h"
#include "string.h"
#include "mem.h"
#include "virtio.h"
#include "ds.h"
#include "fs.h"


int bread(uint32_t dev, uint32_t blockno, void *buffer) {
    struct buf *b = malloc(sizeof(struct buf));

    b->dev = dev;
    b->blockno = blockno;

    virtio_disk_rw(b, 0);
    if(b->successful == 0) return 1;
    memcpy(buffer, (void *)b->data, BLOCK_SIZE);
    free(b);
    return 0;
}

int bwrite(uint32_t dev, uint32_t blockno, void *data) {
    struct buf *b = malloc(sizeof(struct buf));
    b->dev = dev;
    b->blockno = blockno;
    memcpy(b->data, data, BLOCK_SIZE);
    virtio_disk_rw(b, 1);
    int result = b->successful;
    free(b);
    return !result;
}

uint64_t dalloc() {
    block_t block;
    for(uint64_t i = 1; i < MAX_BLOCK; i++) {
        bread(0, i, &block);
        if(block.flag == 0) {
            memset(&block, 0, BLOCK_SIZE);
            block.flag = 1;
            bwrite(0, i, &block);
            return i;
        }
    }
    return MAX_BLOCK;
}

void dfree(uint64_t addr) {
    block_t block;
    memset(&block, 0, BLOCK_SIZE);
    bwrite(0, addr, &block);
}
