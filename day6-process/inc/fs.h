#include "riscv.h"
#include <stddef.h>
#define BLOCK_SIZE 1024

#define MAX_FILENAME_SIZE      256
#define DATA_SIZE (BLOCK_SIZE - sizeof(int8_t) - 3 * sizeof(int64_t))
#define MAX_BLOCK 2048

typedef struct {
    /* 0 -> not initialized
     * 1 -> initialized
     * */
    int8_t flag;
    uint64_t root_directory_block;          // Block number of the root directory.
    unsigned char reserved[1024];           // Make sure the struct is larger than 1 block
} fs_metadata_t;

typedef struct {
    /* 0 -> nothing here
     * 1 -> there is an inode here
     * */
    uint8_t  flag;
	uint8_t  isDirectory;					// Is this inode indicating a directory?
    char 	 name[MAX_FILENAME_SIZE];       // Name of the file.
    uint64_t parent;                        // parent inode
    uint64_t starting_block;                // Starting block of the file.
    uint64_t file_size;                     // Size of the file in bytes.
    uint64_t creation_time;                 // File creation time.
    uint64_t modification_time;             // Last modification time.
    uint64_t access_time;                   // Last access time.
    uint64_t addr;                          // The location of this inode(unit: block)
    unsigned char reserved[1024];           // Make sure the struct is larger than 1 block
} inode_t;

typedef struct {
    /* 0 -> nothing here
     * 1 -> it is referring an inode here
     * 2 -> it is the end of the list, nothing here.
     * */
    uint8_t flag;
    uint64_t addr;
    uint64_t inode_block;
    uint64_t next;
    unsigned char reserved[1024];           // Make sure the struct is larger than 1 block
} ilist_t;

typedef struct {
    int8_t flag;
    unsigned char data[DATA_SIZE];
    uint64_t addr;
    uint64_t prev;
    uint64_t next;	
} block_t;

inline size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

uint64_t dalloc();
void dfree(uint64_t addr);

void format();
void dump_fs();
void dump_directory(inode_t inode, int ident);
void dump_name(inode_t inode, int ident);
int fs_init();
int fs_create(const char *path, const char *filename);
int fs_write(const char *path, const char *data, size_t size);
size_t fs_read(const char *path, char *buffer, size_t size);
int fs_mkdir(const char *path, const char *dirname);
int fs_rm(const char *path);
int fs_rmdir(const char *path);
