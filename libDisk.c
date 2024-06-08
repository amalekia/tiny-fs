#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "tinyFS.h"
#include "TinyFS_errno.h"

#define BLOCKSIZE 256

typedef struct Disk {
    FILE* diskFile;
    char* diskName;
    int diskNum;
    int nBytes;
    int nBlocks;
    struct Disk* next;
} Disk;

static Disk* head = NULL;
static Disk* tail = NULL;

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
    }
}

Disk* findDisk(int disk) {
    Disk* current = head;
    while (current != NULL && current->diskNum != disk) {
        current = current->next;
    }
    return current;
}

Disk* findDiskByName(char* diskName) {
    Disk* current = head;
    while (current != NULL && strcmp(current->diskName, diskName) != 0) {
        current = current->next;
    }
    return current;
}

//return file descriptor
static int diskNum = -1;

int openDisk(char *filename, int nBytes) {
    // increments disk num
    diskNum++;

    FILE* disk;

    if (nBytes < BLOCKSIZE && nBytes != 0) {
        return FILE_OPEN_ERROR;
    }
    
    if (nBytes == 0) {
        //if disk exists reopen it
        if ((disk = fopen(filename, "r+")) == NULL) {
            return FILE_OPEN_ERROR;
        }

        struct stat st;
        if (stat(filename, &st) != 0) {
            return FILE_OPEN_ERROR;
        }
        Disk* newDisk = malloc(sizeof(Disk));
        newDisk->diskFile = disk;
        newDisk->diskName = filename;
        newDisk->diskNum = diskNum;
        newDisk->nBytes = st.st_size;
        newDisk->nBlocks = newDisk->nBytes / BLOCKSIZE;
        newDisk->next = NULL;

        Disk* diskPtr = findDiskByName(filename);
        if (diskPtr != NULL) {
            removeFromDiskList(diskPtr->diskNum);
        }
        addToDiskList(newDisk);
        return diskNum;
    }

    if (nBytes % BLOCKSIZE != 0) {
        nBytes = nBytes - (nBytes % BLOCKSIZE);
    }
    // create the disk file
    if ((disk = fopen(filename, "w+")) == NULL) {
        return FILE_OPEN_ERROR;
    }
    if (truncate(filename, nBytes) != 0) {
        fclose(disk);
        return FILE_OPEN_ERROR;
    }
    Disk* newDisk = malloc(sizeof(Disk));
    newDisk->diskFile = disk;
    newDisk->diskName = filename;
    newDisk->diskNum = diskNum;
    newDisk->nBytes = nBytes;
    newDisk->nBlocks = nBytes / BLOCKSIZE;
    newDisk->next = NULL;
    addToDiskList(newDisk);
    return diskNum;
}

int closeDisk(int disk) {
    Disk* diskPtr = findDisk(disk);
    if (diskPtr == NULL) {
        return ERROR_DISK_NOT_FOUND;
    }
    else {
        if (fclose(diskPtr->diskFile) != 0) {
            return FILE_CLOSE_ERROR;
        }
        removeFromDiskList(disk);
        return FILE_CLOSE_SUCCESS;
    }
}

int readBlock(int disk, int bNum, void *block) {
    Disk* diskPtr = findDisk(disk);
    if (diskPtr == NULL) {
        return ERROR_DISK_NOT_FOUND;
    }

    int offset = bNum * BLOCKSIZE;
    if (fseek(diskPtr->diskFile, offset, SEEK_SET) != 0) {
        return FILE_SEEK_ERROR;
    }
    if (fread(block, BLOCKSIZE, 1, diskPtr->diskFile) < 1) {
        return FILE_READ_ERROR;
    }

    return FILE_READ_SUCCESS;
}

int writeBlock(int disk, int bNum, void *block) {
    Disk* diskPtr = findDisk(disk);
    if (diskPtr == NULL) {
        return ERROR_DISK_NOT_FOUND;
    }

    int offset = bNum * BLOCKSIZE;
    if (fseek(diskPtr->diskFile, offset, SEEK_SET) != 0) {
        return FILE_SEEK_ERROR;
    }
    if (fwrite(block, 1, BLOCKSIZE, diskPtr->diskFile) < 1) {
        return FILE_WRITE_ERROR;
    }

    return FILE_WRITE_SUCCESS;
}