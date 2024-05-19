#include "libTinyFS.h"
#include "libDisk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int tfs_mkfs(char *filename, int nBytes) {
    return 0;
}

int tfs_mount(char *diskname) {
    return 0;
}

int tfs_unmount() {
    return 0;
}

fileDescriptor tfs_openFile(char *name) {
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
