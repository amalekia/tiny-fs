#include "libTinyFS.h"
#include "libDisk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

struct open_file_table {
    int* openDisks;
    int nextFreeSpot;
};

struct disk {
    int diskNum;
    int size;
    struct open_file_table* oft;
};

int tfs_mkfs(char *filename, int nBytes) {
    static int diskNum = -1;

    // open existing disk or creates new disk
    int fd = openDisk(filename, nBytes);

    // check nBytes
    if (nBytes < BLOCKSIZE && nBytes != 0) {
        perror("Disk size is too small");
        return -1;
    }
    if (nBytes % BLOCKSIZE != 0) {
        nBytes = nBytes - (nBytes % BLOCKSIZE);
    }

    diskNum++;
    // setup the disk struct
    struct disk* disk = (struct disk*)malloc(sizeof(struct disk));
    disk->diskNum = diskNum;
    disk->size = nBytes;
    
    // create an open_file_table entry for the disk
    disk->oft = (struct open_file_table*)malloc(sizeof(struct open_file_table));
    disk->oft->openDisks = (int*)malloc(sizeof(int));
    disk->oft->openDisks[diskNum] = fd;
    disk->oft->nextFreeSpot = 1;

    int blockType;
    int magicNumber = 0x44;
    char buffer[BLOCKSIZE];

    for (int i = 0; i < nBytes / BLOCKSIZE; i++) {
        if (i == 0) {           // superblock
            blockType = 0x01;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            writeBlock(fd, i, buffer);
        } else if (i == 1) {    // inode block
            blockType = 0x02;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            writeBlock(fd, i, buffer);
        } else {                // initializing free blocks
            blockType = 0x04;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            writeBlock(fd, i, buffer);
        }
    }

    closeDisk(disk);
    return SUCCESS_TFS_DISK_CREATED;
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
    char buffer[BLOCKSIZE];
    readBlock(disk, 0, &buffer);
    
    // Verify the file system type
    if (buffer[0] != 0x01 || buffer[1] != 0x44) {
        closeDisk(disk);
        return ERROR_INVALID_FILE_SYSTEM;
    }
    
    // Set the mounted disk and return success
    //********TO DO***************
    
    return SUCCESS_TFS_DISK_MOUNTED;
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
    return SUCCESS_TFS_DISK_UNMOUNTED;
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

    return SUCCESS_TFS_OPEN_FILE;
}

int tfs_closeFile(fileDescriptor FD) {
    return SUCCESS_TFS_CLOSE_FILE;
}

int tfs_writeFile(fileDescriptor FD, char *buffer, int size) {
    return SUCCESS_TFS_WRITE_FILE;
}

int tfs_deleteFile(fileDescriptor FD) {
    return SUCCESS_TFS_DELETE_FILE;
}

int tfs_readByte(fileDescriptor FD, char *buffer) {
    return SUCCESS_TFS_READ_FILE;
}

int tfs_seek(fileDescriptor FD, int offset) {
    return SUCCESS_TFS_SEEK_FILE;
}
