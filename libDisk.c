#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "tinyFS.h"
#include "TinyFS_errno.h"

#define BLOCKSIZE 256
#define MAX_FILENAME_LENGTH 256

typedef struct Disk {
    char filename[MAX_FILENAME_LENGTH];
    int diskNum;
    int nBytes;
    int nBlocks;
    struct Disk* next;
} Disk;

Disk* head = NULL;
Disk* tail = NULL;

void addToDiskList(Disk* disk) {
    if (head == NULL) {
        head = disk;
        tail = disk;
    } else {
        tail->next = disk;
        tail = disk;
    }
}

void removeFromDiskList(int disk) {
    if (head->diskNum == disk) {
        head = head->next;
        if (tail->diskNum == disk) {
            tail = NULL;
        }
        free(head); // Free the allocated memory for disk
    } else {
        Disk* current = head;
        Disk* previous = NULL;
        while (current != NULL && current->next->diskNum != disk) {
            previous = current;
            current = current->next;
        }
        if (current != NULL) {
            previous->next = current->next;
            if (tail->diskNum == disk) {
                tail = previous;
            }
            free(current); // Free the allocated memory for disk
        }
        else {
            perror("Disk not found");
        }
    }
}

Disk* findDisk(int disk) {
    Disk* current = head;
    while (current != NULL && current->diskNum != disk) {
        current = current->next;
    }
    if (current != NULL) {
        return current;
    }
    return NULL;
}

//return file descriptor
static int diskNum = -1;

int openDisk(char *filename, int nBytes) {
    FILE* disk;
    if (nBytes < BLOCKSIZE && nBytes != 0) {
        perror("Disk size is too small");
        return FILE_OPEN_ERROR;
    }
    if (nBytes == 0) {
        //if disk exists reopen it
        if ((disk = fopen(filename, "r")) == NULL) {
            perror("Error file doesnt exist");
            return FILE_OPEN_ERROR;
        }
        fclose(disk);
        return diskNum;
    }
    if (nBytes % BLOCKSIZE != 0) {
        nBytes = nBytes - (nBytes % BLOCKSIZE);
    }

    // create the disk file
    if ((disk = fopen(filename, "w+")) == NULL) {
        perror("Error opening file");
        return FILE_OPEN_ERROR;
    }
    // increments disk num and stores it in the open disk array
    diskNum++;
    
    Disk* newDisk = malloc(sizeof(Disk));
    strcpy(newDisk->filename, filename);
    newDisk->diskNum = diskNum;
    newDisk->nBytes = nBytes;
    newDisk->nBlocks = nBytes / BLOCKSIZE;
    newDisk->next = NULL;
    addToDiskList(newDisk);

    fclose(disk);
    return diskNum;
}

int closeDisk(int disk) {
    Disk* diskPtr = findDisk(disk);
    if (diskPtr == NULL) {
        return ERROR_DISK_NOT_FOUND;
    }
    else {
        FILE* diskFile = fopen(diskPtr->filename, "r");
        if (fclose(diskFile) != 0) {
            perror("Error closing file");
            return FILE_CLOSE_ERROR;
        }
        removeFromDiskList(disk);
        return FILE_CLOSE_SUCCESS;
    }
}


int readBlock(int disk, int bNum, void *block) {

    //open disk
    Disk* diskPtr = findDisk(disk);
    if (diskPtr == NULL) {
        return ERROR_DISK_NOT_FOUND;
    }
    
    FILE* diskFile = fopen(diskPtr->filename, "r");

    // calculate the offset and read from that block
    int offset = bNum * BLOCKSIZE;
    if (bNum < 3) {
        perror("Cannot read reserved blocks");
        return FILE_READ_ERROR;
    }
    if (fseek(diskFile, offset, SEEK_SET) != 0) {
        perror("Error seeking to block, invalid block number");
        return FILE_SEEK_ERROR;
    }
    if (fread(block, BLOCKSIZE, 1, diskFile) != 1) {
        perror("Error reading block");
        return FILE_READ_ERROR;
    }

    //close disk
    fclose(diskFile);
    return FILE_READ_SUCCESS;
}


int writeBlock(int disk, int bNum, void *block) {
    //open disk
    Disk* diskPtr = findDisk(disk);
    if (diskPtr == NULL) {
        return ERROR_DISK_NOT_FOUND;
    }
    FILE* diskFile = fopen(diskPtr->filename, "w+");
    // calculate the offset and write to that block
    int offset = bNum * BLOCKSIZE;
    if (bNum < 3) {
        perror("Cannot write to reserved blocks");
        return FILE_WRITE_ERROR;
    }
    if (fseek(diskFile, offset, SEEK_SET) != 0) {
        perror("Error seeking to block, invalid block number");
        return FILE_SEEK_ERROR;
    }
    if (fwrite(block, BLOCKSIZE, 1, diskFile) != 1) {
        perror("Error writing block");
        return FILE_WRITE_ERROR;
    }

    //close disk
    fclose(diskFile);
    return FILE_WRITE_SUCCESS;
}