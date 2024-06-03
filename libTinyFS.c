#include "libTinyFS.h"
#include "libDisk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

// keep track of open files

//keep track of fd for files in filesystem in metadata in list

//in superblock keep track of what blocks are free (bitmap)
// **todo**

struct open_file_table {
    int* openFiles;
};

static int mounted_disk = -1;

int tfs_mkfs(char *filename, int nBytes) {
    // open existing disk or creates new disk
    int disk = openDisk(filename, nBytes);

    int blockType;
    int magicNumber = 0x44;
    char buffer[BLOCKSIZE];

    for (int i = 0; i < nBytes / BLOCKSIZE; i++) {
        if (i == 0) {           // superblock
            blockType = 0x01;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            writeBlock(disk, i, buffer);
        } else if (i == 1) {    // inode block
            blockType = 0x02;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            writeBlock(disk, i, buffer);
        } else {                // initializing free blocks
            blockType = 0x04;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            writeBlock(disk, i, buffer);
        }
    }
    
    closeDisk(disk);
    return SUCCESS_TFS_DISK_CREATED;
}

int isMounted(char *diskname) {
    if (getMountedDisk() != -1) {
        return 1;
    }
    return 0;
}


int tfs_mount(char *diskname) {
    // Check if a file system is already mounted
    if (isMounted(diskname)) {
        return ERROR_ALREADY_MOUNTED;
    }
    
    // Open the disk
    int disk = openDisk(diskname, 0);
    if (disk < 0) {
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
    
    // wirte ot the inode block the filename, size, and pointer to first piece of data (int block num)

    return SUCCESS_TFS_OPEN_FILE;
}

int tfs_closeFile(fileDescriptor FD) {
    return SUCCESS_TFS_CLOSE_FILE;
}

int tfs_writeFile(fileDescriptor FD, char *buffer, int size) {
    int size = sizeof(buffer);

    int totalblocks = (size / BLOCKSIZE);
    if (size % BLOCKSIZE != 0) {
        totalblocks++;
    }

    //if data is bigger than 1 block, set byte 3 ot point to next data block assoicatede with file

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
