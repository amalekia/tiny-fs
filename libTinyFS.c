#include "libTinyFS.h"
#include "libDisk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

// keep track of open files

//keep track of fd for files in filesystem in metadata in list

//in superblock keep track of what blocks are free (bitmap)
// **todo**
#define INODE_SIZE 16

typedef struct TfsFile {
    char filename[8];
    int size;
    int blockNum;
} TfsFile;

TfsFile* head = NULL;
TfsFile* tail = NULL;

static int fd_num = -1;

void addFileToList(char *filename, int size, int blockNum) {
    // Create a new TfsFile struct
    TfsFile *newFile = (TfsFile *)malloc(sizeof(TfsFile));
    strcpy(newFile->filename, filename);
    newFile->size = size;
    newFile->blockNum = blockNum;
    
    // Add the new file to the linked list
    if (head == NULL) {
        head = newFile;
        tail = newFile;
    } else {
        tail->next = newFile;
        tail = newFile;
    }
}

void removeFileFromList(char *filename) {
    // Check if the linked list is empty
    if (head == NULL) {
        return;
    }
    
    // Check if the file to be removed is the head of the linked list
    if (strcmp(head->filename, filename) == 0) {
        TfsFile *temp = head;
        head = head->next;
        free(temp);
        return;
    }
    
    // Traverse the linked list to find the file to be removed
    TfsFile *current = head;
    TfsFile *previous = NULL;
    
    while (current != NULL) {
        if (strcmp(current->filename, filename) == 0) {
            previous->next = current->next;
            free(current);
            return;
        }
        
        previous = current;
        current = current->next;
    }
}

static int mounted_disk = -1;

int tfs_mkfs(char *filename, int nBytes) {
    // creates new disk
    int disk = openDisk(filename, nBytes);

    int blockType;
    int magicNumber = 0x44;
    char buffer[BLOCKSIZE];

    for (int i = 0; i < nBytes / BLOCKSIZE; i++) {
        if (i == 0) {           // superblock
            blockType = 0x01;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            writeBlock(disk, i, buffer);
        } else if (i == 1) {    // inode block
            blockType = 0x02;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            writeBlock(disk, i, buffer);
        } else {                // initializing free blocks
            blockType = 0x04;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            writeBlock(disk, i, buffer);
        }
    }
    closeDisk(disk);
    return SUCCESS_TFS_DISK_CREATED;
}

int tfs_mount(char *diskname) {
    // Open the disk
    int disk = openDisk(diskname, 0);

    // Check if a file system is already mounted
    if (disk == mounted_disk || disk < 0) {
        return ERROR_MOUNTING_FS;
    }
    
    // Verify the file system type
    char buffer[BLOCKSIZE];
    readBlock(disk, 0, &buffer);
    
    if (buffer[0] != 0x01 || buffer[1] != 0x44) {
        closeDisk(disk);
        return ERROR_INVALID_FILE_SYSTEM;
    } else {
        

        // Set the mounted disk and return success
        mounted_disk = disk;
        return SUCCESS_TFS_DISK_MOUNTED;
    }
}


int tfs_unmount() {
    if (closeDisk(mounted_disk) < 0) {
        return ERROR_UNMOUNTING_FS;
    }
    // remove all files in OFT


    mounted_disk = -1;
    return SUCCESS_TFS_DISK_UNMOUNTED;
}

fileDescriptor tfs_openFile(char *name) {
    // Check if a file system is mounted and open it
    if (mounted_disk < 0) {
        return ERROR_NO_FILE_SYSTEM_MOUNTED;
    }

    // Check if the file name is too long
    if (strlen(name) > 8) {
        return ERROR_FILE_NAME_TOO_LONG;
    }
    
    // write to the inode block the filename, size, and 
    // pointer to the first piece of data (int block num)

    // check if file exists in inode block
    // if it does, return the file descriptor
    // if it doesn't, create a new file and return the file descriptor
    // Check if the file exists in the inode block

    int inodeBlockNum = 1; // Assuming inode block is at index 1
    char inodeBuffer[BLOCKSIZE];
    readBlock(mounted_disk, inodeBlockNum, inodeBuffer);
    
    int numFiles = inodeBuffer[0]; // Number of files stored in the inode block
    int fileExists = 0; // Flag to check if the file exists
    
    for (int i = 0; i < numFiles; i++) {
        int offset = i * INODE_SIZE; // Offset to the start of each inode entry
        
        // Check if the file name matches
        if (strncmp(name, &inodeBuffer[offset], FILENAME_SIZE) == 0) {
            fileExists = 1;
            break;
        }
    }
    
    if (fileExists) {
        // File exists, return the file descriptor
        int fileDescriptor = /* Calculate the file descriptor */;
        return fileDescriptor;
    } else {
        // File doesn't exist, create a new file and return the file descriptor
        // TODO: Implement file creation logic
        return /* File descriptor of the newly created file */;
    }

    return SUCCESS_TFS_OPEN_FILE;
}

int tfs_closeFile(fileDescriptor FD) {
    return SUCCESS_TFS_CLOSE_FILE;
}

int tfs_writeFile(fileDescriptor FD, char *buffer, int size) {
    int size = sizeof(buffer);

    int totalblocks = (size / BLOCKSIZE);
    if (size % BLOCKSIZE != 0) {
        totalblocks++;
    }

    //if data is bigger than 1 block, set byte 3 ot point to next data block assoicatede with file

    return SUCCESS_TFS_WRITE_FILE;
}

int tfs_deleteFile(fileDescriptor FD) {
    return SUCCESS_TFS_DELETE_FILE;
}

int tfs_readByte(fileDescriptor FD, char *buffer) {
    return SUCCESS_TFS_READ_FILE;
}

int tfs_seek(fileDescriptor FD, int offset) {
    return SUCCESS_TFS_SEEK_FILE;
}
