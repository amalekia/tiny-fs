#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "TinyFS_errno.h"

#define BLOCKSIZE 256

int openDisk(char *filename, int nBytes) {
    int fd;

    if (nBytes == 0) {
        //if disk exists reopen it
        if ((fd = open(filename, O_RDWR)) == -1) {
            perror("Error opening file");
            return FILE_OPEN_ERROR;
        }
        return diskNum;
    }

    // create the disk file
    if ((fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1) {
        perror("Error opening file");
        return FILE_OPEN_ERROR;
    }
    // returns the disk number on success, error code on failure
    return fd;
}

int closeDisk(int disk) {
    if (close(disk) == -1) {
        perror("Error closing file");
        return FILE_CLOSE_ERROR;
    }
    return FILE_CLOSE_SUCCESS;
}


int readBlock(int disk, int bNum, void *block) {
    int blockAddr = bNum * BLOCKSIZE;

    if (bNum < 3) {
        perror("Cannot read reserved blocks");
        return FILE_READ_ERROR;
    }
    // perform the seek operation
    if (lseek(disk, blockAddr, SEEK_SET) == -1) {
        perror("Error seeking to block, invalid block number");
        return FILE_SEEK_ERROR;
    }
    if (read(disk, block, BLOCKSIZE) == -1) {
        perror("Error reading block");
        return FILE_READ_ERROR;
    }
    // returns 0 on success
    return FILE_READ_SUCCESS;
}


int writeBlock(int disk, int bNum, void *block) {
    struct open_file_table* oft = get_open_file_table(disk);

    int offset = bNum * BLOCKSIZE;
    if (bNum < 3) {
        perror("Cannot write to reserved blocks");
        return FILE_WRITE_ERROR;
    }
    if (lseek(disk, offset, SEEK_SET) == -1) {
        perror("Error seeking to block, invalid block number");
        return FILE_SEEK_ERROR;
    }
    if (write(disk, block, BLOCKSIZE) == -1) {
        perror("Error writing block");
        return FILE_WRITE_ERROR;
    }
    // update next free block
    // *********TO DO***********
    
    // returns 0 on success
    return FILE_WRITE_SUCCESS;
}