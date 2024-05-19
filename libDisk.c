#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BLOCKSIZE 256


int openDisk(char *filename, int nBytes) {
    return 0;
}

int closeDisk(int disk) {
    return 0;
}

int readBlock(int disk, int bNum, void *block) {
    return 0;
}

int writeBlock(int disk, int bNum, void *block) {
    return 0;
}