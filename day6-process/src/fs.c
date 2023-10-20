
#include "fs.h"
#include "bio.h"
#include "string.h"

#ifndef riscvos
#include <stdio.h>
#endif


void format() {
    char zero_block[BLOCK_SIZE] = "";
    for(uint64_t i = 0; i < MAX_BLOCK; i++) {
        bwrite(0, i, &zero_block);
    }
}

void dump_fs() {
    fs_metadata_t superblock;
    bread(0, 0, &superblock);
    inode_t root;
    bread(0, superblock.root_directory_block, &root);
    dump_directory(root, 0);
}

void dump_directory(inode_t inode, int ident) {
    ilist_t ilist;
    bread(0, inode.starting_block, &ilist);
    dump_name(inode, ident);
    while(ilist.flag != 2) {
        inode_t subInode;
        bread(0, ilist.inode_block, &subInode);
        if(subInode.isDirectory) dump_directory(subInode, ident + 2);
        else dump_name(subInode, ident + 2);
        bread(0, ilist.next, &ilist);
    }
}

void dump_name(inode_t inode, int ident) {
    for(int i = 0; i < ident * 2; i++) printf(" ");
    if(inode.name[0] == 0)
        printf("/\n");
    else
        printf("%s\n", inode.name);
}

inode_t* find_inode(const char *path) {
    inode_t* res = malloc(sizeof(inode_t));
    fs_metadata_t superblock;
    bread(0, 0, &superblock);

    superblock.flag = 1;

    if (strcmp(path, "/") == 0) {
        bread(0, superblock.root_directory_block, res);
        return res;
    }

    inode_t currentDirectory, nextDirectory;
    bread(0, superblock.root_directory_block, &currentDirectory);

    char *temp_path = strdup(path);
    char *token = strtok(temp_path, "/");

    while (token != NULL) {
        ilist_t ilist;
        bread(0, currentDirectory.starting_block, &ilist);

        while (ilist.flag != 2) {
            bread(0, ilist.inode_block, &nextDirectory);
            /* printf("%d %d %d, token = %s, name = %s, %d\n", ilist.flag, ilist.addr, ilist.next, token,  nextDirectory.name, strcmp(nextDirectory.name, token)); */
            if (nextDirectory.flag && strcmp(nextDirectory.name, token) == 0) {
                break;
            }
            bread(0, ilist.next, &ilist);
        }

        if(ilist.flag == 2)
            return NULL;

        token = strtok(NULL, "/");
        if (token) {
            currentDirectory = nextDirectory;
        }
    }

    free(temp_path);
    memcpy(res, &nextDirectory, sizeof(inode_t));

    return res;
}

int fs_init() {
    fs_metadata_t superblock;
    bread(0, 0, &superblock);
    if(superblock.flag == 1) return -1;

    // init metadata
    uint64_t rootBlock = dalloc();
    memset(&superblock, 0, sizeof(fs_metadata_t));
    superblock.flag = 1;
    superblock.root_directory_block = rootBlock;

    if(bwrite(0, 0, &superblock))
        return -2;

    // init root directory inode
    uint64_t rootList = dalloc();

    inode_t rootDirectory;
    memset(&rootDirectory, 0, sizeof(inode_t));

    rootDirectory.flag = 1;
    strcpy(rootDirectory.name, "");
    rootDirectory.parent = rootBlock;
    rootDirectory.starting_block = rootList;
    rootDirectory.isDirectory = 1;
    rootDirectory.addr = rootBlock;

    if(bwrite(0, rootBlock, &rootDirectory))
        return -2;

    // init root directory list node
    ilist_t rootDirectoryList;
    memset(&rootDirectoryList, 0, sizeof(ilist_t));
    rootDirectoryList.flag = 2; // mark the list is empty
    rootDirectoryList.addr = rootList;

    if(bwrite(0, rootList, &rootDirectoryList))
        return -2;
    return 0;
}

int fs_create(const char *path, const char *filename) {
    // Step 1: Find the target directory using the provided path.
    inode_t* parentDir = find_inode(path);
    if (parentDir == NULL || parentDir->flag == 0 || parentDir->isDirectory == 0) {
        return -1; // Error: Invalid directory or path not found.
    }

    // Step 2: Check if a file with the same name already exists in the directory.
    ilist_t ilist;
    bread(0, parentDir->starting_block, &ilist);

    while (ilist.flag != 2) {
    	inode_t inode;
        bread(0, ilist.inode_block, &inode);
        if (inode.flag && !inode.isDirectory && strcmp(inode.name, filename) == 0) {
            return -2; // Error: File with the same name already exists.
        }
        bread(0, ilist.next, &ilist);
    }

    // Step 3: Allocate an inode and a data block for the new file.
    uint64_t newInodeBlock = dalloc();
    uint64_t newDataBlock = dalloc();
    if (newInodeBlock == MAX_BLOCK || newDataBlock == MAX_BLOCK) {
        return -3; // Error: No space left on the device.
    }

    // Step 4: Write the metadata for the new file into the allocated inode.
    inode_t newFile;
    memset(&newFile, 0, sizeof(inode_t));
    newFile.flag = 1;
    newFile.addr = newInodeBlock;
    strncpy(newFile.name, filename, MAX_FILENAME_SIZE);
    newFile.parent = parentDir->addr;
    newFile.starting_block = newDataBlock;

    if (bwrite(0, newInodeBlock, &newFile)) {
        return -4; // Error: Failed to write the new inode to disk.
    }
    // Step 5: Update the current block.

    block_t newBlock;
    memset(&newBlock, 0, sizeof(block_t));
    newBlock.flag = 1;
    newBlock.addr = newDataBlock;
    newBlock.next = 0;
    newBlock.prev = 0;
    memset(newBlock.data, 0, DATA_SIZE);

    if (bwrite(0, newDataBlock, &newBlock)) {
        return -4; // Error: Failed to write the new inode to disk.
    }

    // Step 6: Update the parent directory to include a reference to the new file.
    ilist_t newIlist;
    memset(&newIlist, 0, sizeof(ilist_t));
    newIlist.flag = 2;

    bread(0, parentDir->starting_block, &ilist);
    while (ilist.flag != 2) {
        bread(0, ilist.next, &ilist);
    }
    newIlist.flag = 2;
    newIlist.addr = dalloc();

    ilist.flag = 1;
    ilist.inode_block = newInodeBlock;
    ilist.next = newIlist.addr;

    if (bwrite(0, ilist.addr, &ilist)) {
        return -5; // Error: Failed to update the parent directory.
    }

    if (bwrite(0, newIlist.addr, &newIlist)) {
        return -6; // Error: Failed to update the parent directory.
    }

    free(parentDir);

    return 0;
}

int fs_write(const char *path, const char *data, size_t size) {
    // Step 1: Find the target file using the provided path and filename.
    inode_t* targetFile = find_inode(path);

    if (targetFile == NULL || !targetFile->flag || targetFile->isDirectory) {
        return -1; // Error: Invalid file or file not found.
    }

    // Step 2: Write the data to the target file's data block.

    block_t block;
    bread(0, targetFile->starting_block, &block);
    size_t remain_size = size;
    size_t offset = 0;
	while (remain_size) {
    	size_t copy_size = remain_size < DATA_SIZE ? remain_size : DATA_SIZE;
		remain_size -= copy_size;
        /* printf("write %d %d %d, rm_size %d, cp_size %d\n", block.flag, block.addr, block.next, remain_size, copy_size); */
		memcpy(block.data, data + offset, copy_size);
		if(remain_size) {
            if(block.next == 0) {
                block_t newBlock;
                newBlock.flag = 1;
                newBlock.addr = dalloc();
                newBlock.prev = block.addr;
                newBlock.next = 0;
                block.next = newBlock.addr;
                bwrite(0, block.addr, &block);
                block = newBlock;
                /* printf("%s", buffer); */
            } else {
                bwrite(0, block.addr, &block);
                bread(0, block.next, &block);
            }
		} else {
    		bwrite(0, block.addr, &block);
		}
        offset += copy_size;
    }

    // Step 3: Update the file size in the inode (assuming you have a file size field in your inode structure).
    targetFile->file_size = size; // Uncomment this line if you have a size field in your inode structure.
    bwrite(0, targetFile->addr, targetFile); // Save the updated inode to disk.

    free(targetFile);

    // Step 4: Return success.
    return 0;
}

size_t fs_read(const char *path, char *buffer, size_t size) {
    // Step 1: Find the target file using the provided path.
    inode_t* targetFile = find_inode(path);
    if (targetFile == NULL || !targetFile->flag || targetFile->isDirectory) {
        return -1; // Error: Invalid file or file not found.
    }

    // Step 2: Check if the requested size is more than the file's size.
    // If yes, update the size to be read to the file's size.
    if (size > targetFile->file_size) {
        size = targetFile->file_size;
    }

    // Step 3: Read the data from the target file's data blocks.
    block_t block;
    bread(0, targetFile->starting_block, &block);

    size_t total_read = 0;
    while (size > 0) {
        /* printf("%d %d %d\n", block.flag, block.addr, block.next); */
        size_t bytes_to_read = size < DATA_SIZE ? size : DATA_SIZE;

        /* printf("%lld, writing %lld\n", (long long)size, (long long)bytes_to_read); */
        memcpy(buffer + total_read, block.data, bytes_to_read);
        total_read += bytes_to_read;
        size -= bytes_to_read;

        // If there's more to read, move on to the next block.
        if (size) {
            if (block.next == 0) {
                // No more blocks to read.
                break;
            }
            bread(0, block.next, &block);
        }
    }

    free(targetFile);

    // Step 4: Return the total number of bytes read.
    return total_read;
}

int fs_mkdir(const char *path, const char *dirname) {
    // Step 1: Find the target directory using the provided path.
    inode_t* parentDir = find_inode(path);
    if (path == NULL || parentDir->flag == 0 || parentDir->isDirectory == 0) {
        return -1; // Error: Invalid directory or path not found.
    }

    // Step 2: Check if a directory with the same name already exists in the directory.
    ilist_t ilist;
    bread(0, parentDir->starting_block, &ilist);

    while (ilist.flag != 2) {
        inode_t inode;
        bread(0, ilist.inode_block, &inode);
        if (inode.flag && inode.isDirectory && strcmp(inode.name, dirname) == 0) {
            return -2; // Error: Directory with the same name already exists.
        }
        bread(0, ilist.next, &ilist);
    }

    // Step 3: Allocate an inode and a data block for the new directory.
    uint64_t newInodeBlock = dalloc();
    uint64_t newListBlock = dalloc();
    if (newInodeBlock == MAX_BLOCK || newListBlock == MAX_BLOCK) {
        return -3; // Error: No space left on the device.
    }

    // Step 4: Write the metadata for the new directory into the allocated inode.
    inode_t newDir;
    memset(&newDir, 0, sizeof(inode_t));
    newDir.flag = 1;
    newDir.addr = newInodeBlock;
    strncpy(newDir.name, dirname, MAX_FILENAME_SIZE);
    newDir.parent = parentDir->addr;
    newDir.starting_block = newListBlock;
    newDir.isDirectory = 1;

    if (bwrite(0, newInodeBlock, &newDir)) {
        return -4; // Error: Failed to write the new inode to disk.
    }

    // Step 5: Initialize the directory list node for the new directory as empty.
    ilist_t newDirList;
    memset(&newDirList, 0, sizeof(ilist_t));
    newDirList.flag = 2;  // mark the list as empty
    newDirList.addr = newListBlock;

    if (bwrite(0, newListBlock, &newDirList)) {
        return -5; // Error: Failed to write the new directory list to disk.
    }

    // Step 6: Update the parent directory to include a reference to the new directory.
    ilist_t newIlist;
    memset(&newIlist, 0, sizeof(ilist_t));
    newIlist.flag = 2;

    bread(0, parentDir->starting_block, &ilist);
    while (ilist.flag != 2) {
        bread(0, ilist.next, &ilist);
    }
    newIlist.flag = 2;
    newIlist.addr = dalloc();

    ilist.flag = 1;
    ilist.inode_block = newInodeBlock;
    ilist.next = newIlist.addr;

    if (bwrite(0, ilist.addr, &ilist)) {
        return -6; // Error: Failed to update the parent directory.
    }

    if (bwrite(0, newIlist.addr, &newIlist)) {
        return -7; // Error: Failed to update the parent directory.
    }

    free(parentDir);

    return 0;
}

int fs_rm(const char *path) {
    // Step 1: Find the target file using the provided path.
    inode_t* targetFile = find_inode(path);
    if (targetFile == NULL || !targetFile->flag || targetFile->isDirectory) {
        free(targetFile);
        return -1; // Error: Invalid file or file not found.
    }

    // Step 2: Clear the file's data blocks.
    block_t block;
    bread(0, targetFile->starting_block, &block);

    while (block.flag) {
        uint64_t next = block.next;
        dfree(block.addr);
        if (next == 0) break;
        bread(0, next, &block);
    }

    // Step 3: Remove the inode reference from the parent directory list.
    inode_t parentDir;
    bread(0, targetFile->parent, &parentDir);

    ilist_t ilist, prev_ilist;
    memset(&prev_ilist, 0, sizeof(ilist_t));

    bread(0, parentDir.starting_block, &ilist);

    while (ilist.flag != 2) {
        if (ilist.inode_block == targetFile->addr) {
            if (prev_ilist.flag) {
                prev_ilist.next = ilist.next;
                bwrite(0, prev_ilist.addr, &prev_ilist);
            } else {
                parentDir.starting_block = ilist.next;
                bwrite(0, parentDir.addr, &parentDir);
            }
            dfree(ilist.addr);
            break;
        }
        prev_ilist = ilist;
        bread(0, ilist.next, &ilist);
    }

    // Step 4: Free the target file's inode block.
    dfree(targetFile->addr);

    free(targetFile);

    return 0;
}

int fs_rmdir(const char *path) {
    // Step 1: Find the target directory using the provided path.
    inode_t* targetDir = find_inode(path);
    if (targetDir == NULL || !targetDir->flag || !targetDir->isDirectory) {
        free(targetDir);
        return -1; // Error: Invalid directory or directory not found.
    }

    // Step 2: Ensure the directory is empty.
    ilist_t ilist;
    bread(0, targetDir->starting_block, &ilist);
    if (ilist.flag != 2) {
        free(targetDir);
        return -2; // Error: Directory is not empty.
    }

    // Step 3: Remove the inode reference from the parent directory list.
    inode_t parentDir;
    bread(0, targetDir->parent, &parentDir);

    ilist_t prev_ilist;
    memset(&prev_ilist, 0, sizeof(ilist_t));

    bread(0, parentDir.starting_block, &ilist);

    while (ilist.flag != 2) {
        if (ilist.inode_block == targetDir->addr) {
            if (prev_ilist.flag) {
                prev_ilist.next = ilist.next;
                bwrite(0, prev_ilist.addr, &prev_ilist);
            } else {
                parentDir.starting_block = ilist.next;
                bwrite(0, parentDir.addr, &parentDir);
            }
            dfree(ilist.addr);
            break;
        }
        prev_ilist = ilist;
        bread(0, ilist.next, &ilist);
    }

    // Step 4: Free the target directory's inode block.
    dfree(targetDir->addr);

    free(targetDir);

    return 0;
}
