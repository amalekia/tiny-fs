#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "tinyFS_errno.h"

#define BLOCKSIZE 256


int openDisk(char *filename, int nBytes) {
    int diskSize;
    int remainder;

    if (nBytes < BLOCKSIZE && nBytes != 0) {
        perror("Disk size is too small");
        return -1;
    }
    else if (nBytes == 0) {
        fd = open(filename, O_RDWR);
        return fd;
    }
    else if ((remainder = nBytes % BLOCKSIZE) != 0) {
        diskSize = nBytes - remainder;
    }
    else {
        diskSize = nBytes;
    }

    // open or create the disk file
    fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error opening file");
        return FILE_OPEN_ERROR;
    }

    // allocate space for the disk
    if (ftruncate(fd, diskSize) == -1) {
        perror("Error allocating space for disk");
        return FILE_OPEN_ERROR;
    }

    // returns the disk number on success, -1 on failure
    return diskNum;
}

int closeDisk(int disk) {
    return 0;
}

int readBlock(int disk, int bNum, void *block) {
    offset = bNum * BLOCKSIZE;
    
    if (fseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking to block, invalid block number");
        return FILE_SEEK_ERROR;
    }

    if (fread(block, BLOCKSIZE, 1, fd) == 0) {
        perror("Nothing read, error reading block");
        return FILE_READ_ERROR;
    }

    // returns 0 on success
    return 0;
}

int writeBlock(int disk, int bNum, void *block) {
    offset = bNum * BLOCKSIZE;

    if (fseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking to block, invalid block number");
        return FILE_SEEK_ERROR;
    }

    if (fwrite(block, BLOCKSIZE, 1, fd) == 0) {
        perror("Nothing written, error writing block");
        return FILE_WRITE_ERROR;
    }
    // returns 0 on success
    return 0;
}