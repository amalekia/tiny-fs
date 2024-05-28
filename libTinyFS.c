#include "libTinyFS.h"
#include "libDisk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int tfs_mkfs(char *filename, int nBytes) {
    int disk = openDisk(filename, nBytes);

    // int blockType = 0x01;
    // int magicNumber = 0x44;
    // // write the superblock
    // writeBlock(disk, 0, &blockType);
    // writeBlock(disk, 0, &magicNumber); // need to have a tracker to see next free spot to write in block
    
    // initialize and write the superblock
    Superblock superblock;
    superblock.blockType = 0x01;
    superblock.magicNumber = 0x44;
    writeBlock(disk, 0, &superblock);

    // initialize and write the inodes
    Inode inodes[NUM_INODES];
    memset(inodes, 0, sizeof(Inode) * NUM_INODES);
    writeBlock(disk, 1, inodes);

    // initialize and write the data blocks
    char dataBlocks[NUM_BLOCKS][BLOCK_SIZE];
    memset(dataBlocks, 0, sizeof(char) * NUM_BLOCKS * BLOCK_SIZE);
    for (int i = 0; i < NUM_BLOCKS; i++) {
        writeBlock(disk, i + 2, dataBlocks[i]);
    }

    closeDisk(disk);
    return 0;
}

int tfs_mount(char *diskname) {
    // Check if a file system is already mounted
    if (isMounted()) {
        return ERROR_ALREADY_MOUNTED;
    }
    
    // Open the disk
    int disk = openDisk(diskname, 0);
    if (disk == -1) {
        return ERROR_DISK_OPEN_FAILED;
    }
    
    // Read the superblock
    Superblock superblock;
    readBlock(disk, 0, &superblock);
    
    // Verify the file system type
    if (superblock.blockType != 0x01 || superblock.magicNumber != 0x44) {
        closeDisk(disk);
        return ERROR_INVALID_FILE_SYSTEM;
    }
    
    // Set the mounted disk and return success
    setMountedDisk(disk);
    return SUCCESS;
}


int tfs_unmount() {
    // Check if a file system is mounted
    if (!isMounted()) {
        return ERROR_NO_FILE_SYSTEM_MOUNTED;
    }
    
    // Close the mounted disk
    int disk = getMountedDisk();
    closeDisk(disk);
    
    // Clear the mounted disk and return success
    clearMountedDisk();
    return SUCCESS;
}

fileDescriptor tfs_openFile(char *name) {
    if (!isMounted()) {
        return ERROR_NO_FILE_SYSTEM_MOUNTED;
    }

    // Check if the name is too long
    if (strlen(name) > 8) {
        return ERROR_FILE_NAME_TOO_LONG;
    }
    
    // Read the inodes
    Inode inodes[NUM_INODES];

    return 0;
}

int tfs_closeFile(fileDescriptor FD) {
    return 0;
}

int tfs_writeFile(fileDescriptor FD, char *buffer, int size) {
    return 0;
}

int tfs_deleteFile(fileDescriptor FD) {
    return 0;
}

int tfs_readByte(fileDescriptor FD, char *buffer) {
    return 0;
}

int tfs_seek(fileDescriptor FD, int offset) {
    return 0;
}
