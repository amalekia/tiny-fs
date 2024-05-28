#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "TinyFS_errno.h"

#define BLOCKSIZE 256

struct open_file_table {
    int* openDisks;
    int nextFreeSpot;
};

struct disk {
    int diskNum;
    int size;
    struct open_file_table* oft;
};

int openDisk(char *filename, int nBytes) {
    int fd;
    static int diskNum = -1;

    if (nBytes == 0) {
        //if disk exists reopen it
        if ((fd = open(filename, O_RDWR)) == -1) {
            perror("Error opening file");
            return FILE_OPEN_ERROR;
        }
        diskNum++;
        return diskNum;
    }

    //else create a new disk file and check nBytes
    if (nBytes < BLOCKSIZE && nBytes != 0) {
        perror("Disk size is too small");
        return -1;
    }
    if (nBytes % BLOCKSIZE != 0) {
        nBytes = nBytes - (nBytes % BLOCKSIZE);
    }

    // create the disk file
    if ((fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1) {
        perror("Error opening file");
        return FILE_OPEN_ERROR;
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

    // returns the disk number on success, error code on failure
    return diskNum;
}

int closeDisk(int disk) {
    struct open_file_table* oft = get_open_file_table(disk);
    if (close(oft->openDisks[disk]) == -1) {
        perror("Error closing file");
        return FILE_CLOSE_ERROR;
    }
    free(oft->openDisks);
    free(oft);
    return FILE_CLOSE_SUCCESS;
}


int readBlock(int disk, int bNum, void *block) {
    struct open_file_table* oft = get_open_file_table(disk);

    int blockAddr = bNum * BLOCKSIZE;

    if (bNum < 3) {
        perror("Cannot read reserved blocks");
        return FILE_READ_ERROR;
    }
    // perform the seek operation
    if (lseek(oft->openDisks[disk], blockAddr, SEEK_SET) == -1) {
        perror("Error seeking to block, invalid block number");
        return FILE_SEEK_ERROR;
    }
    if (read(oft->openDisks[disk], block, BLOCKSIZE) == -1) {
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
    if (lseek(oft->openDisks[disk], offset, SEEK_SET) == -1) {
        perror("Error seeking to block, invalid block number");
        return FILE_SEEK_ERROR;
    }
    if (write(oft->openDisks[disk], block, BLOCKSIZE) == -1) {
        perror("Error writing block");
        return FILE_WRITE_ERROR;
    }
    // update next free block
    // *********TO DO***********
    
    // returns 0 on success
    return FILE_WRITE_SUCCESS;
}