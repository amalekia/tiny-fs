#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "tinyFS.h"
#include "TinyFS_errno.h"
#include "libTinyFS.h"

int main(int argc, char *argv[]) {
    int errno;
    if ((errno = tfs_mkfs("tinyFSDemo", 10240)) < 0) {
        printf("Error creating file system: %d\n", errno);
        exit(1);
    }
    if ((errno = tfs_mount("tinyFSDemo")) < 0) {
        printf("Error mounting file system: %d\n", errno);
        exit(1);
    }
    int file = tfs_openFile("tinyFSDemo");
    if (file < 0) {
        printf("Error opening file\n");
        exit(1);
    }
    char *buffer = (char *)malloc(1024);
    for (int i = 0; i < 1024; i++) {
        buffer[i] = 'a';
    }
    if (tfs_writeFile(file, buffer, 1024) < 0) {
        printf("Error writing to file\n");
        exit(1);
    }
    tfs_seek(file, 0);
    printf("Reading from file\n");
    char *readBuffer = (char *)malloc(1024);
    tfs_readByte(file, readBuffer);
    for (int i = 0; i < 1024; i++) {
        printf("%c", readBuffer[i]);
    }
    printf("\n");
    tfs_deleteFile(file);
    return 0;
}