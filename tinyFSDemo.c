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
    //test 1
    if ((errno = tfs_mkfs("tinyFSDisk", 10240)) < 0) {
        printf("Error creating file system: %d\n", errno);
        exit(1);
    }
    else {
        printf("File system created successfully\n");
    }
    //test 2
    if ((errno = tfs_mount("tinyFSDisk")) < 0) {
        printf("Error mounting file system: %d\n", errno);
        exit(1);
    }
    else {
        printf("File system mounted successfully\n");
    }
    //test 3
    int file = tfs_openFile("file1");
    if (file < 0) {
        printf("Error opening file: %d\n", file);
        exit(1);
    }
    else {
        printf("File 1 opened successfully\n");
    }
    //test 4
    char *buffer = (char *)malloc(1024);
    for (int i = 0; i < 1024; i++) {
        buffer[i] = 'a';
    }
    if ((errno = tfs_writeFile(file, buffer, 1024)) < 0) {
        printf("Error writing to file: %d\n", errno);
        exit(1);
    }
    else {
        printf("File1 written to successfully\n");
    }
    //test 5
    if ((errno = tfs_seek(file, 25)) < 0) {
        printf("Error seeking in file: %d\n", errno);
        exit(1);
    }
    else {
        printf("Seeked to spot 25 of file 1\n");
    }
    if ((errno = tfs_seek(file, 0)) < 0) {
        printf("Error seeking in file: %d\n", errno);
        exit(1);
    }
    else {
        printf("Seeked to beginning of file 1\n");
    }
    if ((errno = tfs_writeByte(file, 350, 'F')) < 0) {
        printf("Error writing to file: %d\n", errno);
        exit(1);
    }
    else {
        printf("Byte written to file 1 successfully\n");
    }
    //test 6
    if ((errno = tfs_seek(file, 0)) < 0) {
        printf("Error seeking in file: %d\n", errno);
        exit(1);
    }
    else {
        printf("Seeked to beginning of file 1\n");
    }
    //test 7
    printf("Reading from file 1\n");
    char *readBuffer = (char *)malloc(1);
    for (int i = 0; i < 1024; i++) {
        if ((errno = tfs_readByte(file, readBuffer)) < 0) {
            printf("Error reading from file: %d\n", errno);
            exit(1);
        }
        else {
            printf("%c", readBuffer[0]);
            memset(readBuffer, 0, 1);
        }
    }
    printf("\n");
    printf("Succesfully read 1024 bytes scattered across multiple data blocks in file 1\n");

    //making file 2
    int file2 = tfs_openFile("file2");
    if (file2 < 0) {
        printf("Error opening file: %d\n", file);
        exit(1);
    }
    else {
        printf("File 2 opened successfully\n");
    }
    //test 8
    if ((errno = tfs_readdir()) < 0) {
        printf("Error reading directory: %d\n", errno);
        exit(1);
    }
    else {
        printf("Directory read successfully\n");
    }
    if ((errno = tfs_rename(file, "file3")) < 0) {
        printf("Error renaming file: %d\n", errno);
        exit(1);
    }
    else {
        printf("File 'file1' renamed to 'file3' successfully\n");
    }
    if ((errno = tfs_readdir()) < 0) {
        printf("Error reading directory: %d\n", errno);
        exit(1);
    }
    else {
        printf("Directory read successfully\n");
    }
    //test 9
    if ((errno = tfs_deleteFile(file)) < 0) {
        printf("Error deleting file: %d\n", errno);
        exit(1);
    }
    else {
        printf("'file3' deleted successfully\n");
    }

    if ((errno = tfs_readdir()) < 0) {
        printf("Error reading directory: %d\n", errno);
        exit(1);
    }
    else {
        printf("Directory read successfully\n");
    }
    //test 11
    if ((errno = tfs_makeRO("file2")) < 0) {
        printf("Error making file read only: %d\n", errno);
        exit(1);
    }
    else {
        printf("File 2 made read only successfully\n");
    }
    //test 12
    if ((errno = tfs_writeFile(file2, buffer, 1024)) < 0) {
        printf("Success, error writing to file 2 in RO mode\n");
    }
    else {
        printf("Error writing to file: %d\n", errno);
        exit(1);
    }
    //test 13
    if ((errno = tfs_makeRW("file2")) < 0) {
        printf("Error making file read write: %d\n", errno);
        exit(1);
    }
    else {
        printf("File 2 made read write successfully\n");
    }
    //test 14
    char *cbuffer = (char *)malloc(252);
    for (int i = 0; i < 252; i++) {
        buffer[i] = 'a' + (i % 26);
    }
    if ((errno = tfs_writeFile(file2, cbuffer, 1024)) < 0) {
        printf("Error writing to file: %d\n", errno);
        exit(1);
    }
    else {
        printf("File1 written to successfully\n");
    }

    if ((errno = tfs_closeFile(file2)) < 0) {
        printf("Error closing file: %d\n", errno);
        exit(1);
    }
    else {
        printf("'file2' closed successfully\n");
    }
    //test 10
    file2 = tfs_openFile("file2");
    if (file2 < 0) {
        printf("Error opening file: %d\n", file);
        exit(1);
    }
    else {
        printf("File 2 reopened successfully. fd: %d\n", file2);
    }
    
    if ((errno = tfs_unmount()) < 0) {
        printf("Error unmounting file system: %d\n", errno);
        exit(1);
    }
    else {
        printf("File system unmounted successfully\n");
    }
    return 0;
}