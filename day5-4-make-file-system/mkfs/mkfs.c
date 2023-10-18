#include "fs.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define FILE_NAME "disk.img"
#define MAX_SIZE 2048



int bwrite(int dev, uint64_t no, void *buffer) {
    FILE *fp = fopen(FILE_NAME, "rb+");
    if (!fp) {
        perror("Error opening file");
        return -1;
    }

    fseek(fp, no * BLOCK_SIZE, SEEK_SET);

    printf("writing to %ld, length = %d\n", no * BLOCK_SIZE, BLOCK_SIZE);
    size_t writeSize = fwrite((char*)buffer, 1, BLOCK_SIZE, fp);
    fclose(fp);

    if (writeSize != BLOCK_SIZE) {
        perror("Error writing block");
        return -1;
    }

    return 0;  // 成功
}

int bread(int dev, uint64_t no, void *buffer) {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) {
        perror("Error opening file");
        return -1;
    }

    fseek(fp, no * BLOCK_SIZE, SEEK_SET);

    size_t readSize = fread((char*)buffer, 1, BLOCK_SIZE, fp);
    fclose(fp);

    if (readSize != BLOCK_SIZE) {
        perror("Error reading block");
        return -1;
    }

    return 0;
}

uint64_t dalloc() {

    char buffer[BLOCK_SIZE];
    block_t temp;
    memset(&temp, 1, sizeof(block_t));
    uint64_t addr = 1;
    while (bread(0, addr, buffer) == 0) {
        int allZero = 1;
        for (int32_t i = 0; i < BLOCK_SIZE; i++) {
            if (buffer[i] != 0) {
                allZero = 0;
                break;
            }
        }
        if (allZero) {
            printf("alloc %ld\n", addr);
            printf("%d\n", bwrite(0, addr, &temp));
            return addr;
        }
        addr++;
    }

    return MAX_SIZE;
}

void dfree(uint64_t addr) {
    FILE *fp = fopen(FILE_NAME, "r+b");
    if (!fp) {
        perror("Error opening file");
        return;
    }

    char buffer[BLOCK_SIZE];
    memset(buffer, 0, BLOCK_SIZE);
    
    fseek(fp, addr * BLOCK_SIZE, SEEK_SET);
    fwrite(buffer, 1, BLOCK_SIZE, fp);

    fclose(fp);
}

void addFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the file content
    char *buffer = (char *)malloc(size);
    if (!buffer) {
        perror("Memory allocation failed");
        fclose(file);
        return;
    }

    // Read file into the buffer
    size_t bytesRead = fread(buffer, 1, size, file);
    fclose(file);

    if (bytesRead != size) {
        perror("Error reading the file");
        free(buffer);
        return;
    }


    /* printf("->%d\n", fs_create("/", "test")); */
    fs_create("/", filename);
	char path[257] = "/";
	strcpy(path + 1, filename);
	printf("Add %s\n", filename);
	fs_write(path, buffer, size);
    free(buffer);
}


int main(int argc, char *argv[]) {
    if(fs_init() == 0) {
        for (int i = 1; i < argc; i++) {
            addFile(argv[i]);
        }

        dump_fs();
    } else {
        printf("not empty disk.\n");
    }
    return 0;
}
