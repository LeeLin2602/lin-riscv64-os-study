#include "bio.h"

int bread(uint32_t dev, uint32_t blockno, char * buffer) {
    struct buf *b = malloc(sizeof(struct buf));

    b->dev = dev;
    b->blockno = blockno;

    virtio_disk_rw(b, 0);
    if(b->successful == 0) return 0;
    memcpy(buffer, (void *)b->data, BLOCK_SIZE);
    free(b);
    return 1;
}

int bwrite(uint32_t dev, uint32_t blockno, const char *data) {
    struct buf *b = malloc(sizeof(struct buf));
    b->dev = dev;
    b->blockno = blockno;
    memcpy(b->data, data, BLOCK_SIZE);
    virtio_disk_rw(b, 1);
    int result = b->successful;
    free(b);
    return result;
}

