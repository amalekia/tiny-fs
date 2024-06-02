#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
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

int openDisk(char *filename, int nBytes);
int closeDisk(int disk);
int readBlock(int disk, int bNum, void *block);
int writeBlock(int disk, int bNum, void *block);
void addToDiskList(Disk* disk);
Disk* findDisk(int disk);
void removeFromDiskList(int disk);