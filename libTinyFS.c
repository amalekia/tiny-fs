#include "libTinyFS.h"
#include "libDisk.h"
#include "tinyFS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

//keep track of fd for files in filesystem in metadata in list

static int fd_num = -1;
static int mounted_disk = -1;

typedef struct TfsFile {
    char filename[8];
    int fd;
    int size;
    int inodeBlock;
    int filePointer;
    struct TfsFile *next;
} TfsFile;

static TfsFile* headFile = NULL;
static TfsFile* tailFile = NULL;

void addFileToOFT(char *filename, int fd, int size, int inodeBlock, int filePointer) {
    // Create a new TfsFile struct
    TfsFile *newFile = (TfsFile *)malloc(sizeof(TfsFile));
    strcpy(newFile->filename, filename);
    newFile->fd = fd;
    newFile->size = size;
    newFile->inodeBlock = inodeBlock;
    newFile->filePointer = filePointer;
    newFile->next = NULL;
    
    // Add the new file to the linked list
    if (headFile == NULL) {
        headFile = newFile;
        tailFile = newFile;
    } else {
        tailFile->next = newFile;
        tailFile = newFile;
    }
}

TfsFile* findFileNameInList(char *filename) {
    // Traverse the linked list to find the file
    TfsFile *current = headFile;
    
    while (current != NULL) {
        if (strcmp(current->filename, filename) == 0) {
            return current;
        }
        
        current = current->next;
    }
    
    return NULL;
}

TfsFile* findFileFDInList(int fd) {
    // Traverse the linked list to find the file
    TfsFile *current = headFile;
    
    while (current != NULL) {
        if (current->fd == fd) {
            return current;
        }
        
        current = current->next;
    }
    
    return NULL;
}

void removeFileFromOFT(int fd) {
    // Check if the linked list is empty
    if (headFile == NULL) {
        return;
    }
    
    // Check if the file to be removed is the head of the linked list
    if (headFile->fd == fd) {
        TfsFile *temp = headFile;
        headFile = headFile->next;
        free(temp);
        return;
    }
    
    // Traverse the linked list to find the file to be removed
    TfsFile *current = headFile;
    TfsFile *previous = NULL;
    
    while (current != NULL) {
        if (current->fd == fd) {
            previous->next = current->next;
            free(current);
            return;
        }
        
        previous = current;
        current = current->next;
    }
}

int tfs_mkfs(char *filename, int nBytes) {
    // creates new disk
    int disk = openDisk(filename, nBytes);
    printf("disk: %d\n", disk);

    int blockType;
    char buffer[BLOCKSIZE];

    for (int i = 0; i < nBytes / BLOCKSIZE; i++) {
        if (i == 0) {           // superblock
            blockType = 0x01;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            buffer[2] = i + 2;
            buffer[4] = nBytes / BLOCKSIZE;
            printf("superblock buffer[0]: %d\n", buffer[0]);
            printf("superblock buffer[1]: %d\n", buffer[1]);
            printf("superblock buffer[2]: %d\n", buffer[2]);
            printf("superblock buffer[4]: %d\n", buffer[4]);
            int ret;
            if ((ret = writeBlock(disk, i, buffer)) < 0) {
                return ret;
            }
            readBlock(disk, i, &buffer);
            printf("buffer[0]: %d\n", buffer[0]);
            printf("buffer[1]: %d\n", buffer[1]);
            printf("buffer[2]: %d\n", buffer[2]);
            printf("buffer[4]: %d\n", buffer[4]);
        } 
        else if (i == 1) {      // root inode block
            blockType = 0x02;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            buffer[2] = 0;
            writeBlock(disk, i, buffer);
            memset(buffer, 0, BLOCKSIZE);
            readBlock(disk, 0, &buffer);
            printf("buffer[0] : %d\n", buffer[0]);
            printf("buffer[1] : %d\n", buffer[1]);
            printf("buffer[2] : %d\n", buffer[2]);
            printf("buffer[4] : %d\n", buffer[4]);
        }
        else {                // initializing free blocks
            blockType = 0x04;
            memset(buffer, 0, BLOCKSIZE);
            buffer[0] = blockType;
            buffer[1] = 0x44;
            if (i == nBytes / BLOCKSIZE - 1) {
                buffer[2] = 0;
            } else {
                buffer[2] = i + 1;
            }
            writeBlock(disk, i, buffer);
        }
    }

    closeDisk(disk);
    return SUCCESS_TFS_DISK_CREATED;
}

int tfs_unmount() {
    if (closeDisk(mounted_disk) < 0) {
        return ERROR_UNMOUNTING_FS;
    }
    // remove all files in OFT
    while (headFile != NULL) {
        TfsFile *temp = headFile;
        headFile = headFile->next;
        free(temp);
    }
    mounted_disk = -1;
    return SUCCESS_TFS_DISK_UNMOUNTED;
}

int tfs_mount(char *diskname) {
    // Open the disk
    int disk = openDisk(diskname, 0);

    // Check if a file system is already mounted
    if (disk == mounted_disk || disk < 0) {
        return ERROR_MOUNTING_FS;
    } else if (mounted_disk >= 0) {
        tfs_unmount();
    }
    
    // Verify the file system type
    char buffer[BLOCKSIZE];
    printf("Open disk: %d\n", disk);
    readBlock(disk, 0, &buffer);

    printf("buffer[0]: %x\n", buffer[0]);
    printf("buffer[1]: %x\n", buffer[1]);
    
    if (buffer[0] != 0x01 && buffer[1] != 0x44) {
        closeDisk(disk);
        return ERROR_INVALID_FILE_SYSTEM;
    } else {
        // Set the mounted disk and return success
        mounted_disk = disk;
        return SUCCESS_TFS_DISK_MOUNTED;
    }
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

    // Check if the file already exists
    TfsFile *file = findFileNameInList(name);
    if (file != NULL) {
        return file->fd;
    } 
    else {
        // Find the root directory and locate inode block num
        char buffer[BLOCKSIZE];
        readBlock(mounted_disk, 1, buffer);
        int fileInodeBlock = -1;
        int i = 4;
        while (buffer[i] != 0) {
            if (strcmp(buffer + i, name) == 0) {
                fileInodeBlock = buffer[i + 9];
                break;
            }
            i++;
        }
        if (fileInodeBlock == -1) { //if file does not exist in root directory
            // Create a new inode block for the file
            readBlock(mounted_disk, 0, buffer);
            fileInodeBlock = buffer[2];
            if (fileInodeBlock == 0) {
                return ERROR_NOT_ENOUGH_FREE_BLOCKS;
            }
            else {
                // write new free block to superblock
                readBlock(mounted_disk, fileInodeBlock, buffer);
                int newFreeBlock = buffer[2];
                readBlock(mounted_disk, 0, buffer);
                buffer[2] = newFreeBlock;
                writeBlock(mounted_disk, 0, buffer);

                // write new inode block to root directory
                readBlock(mounted_disk, 1, buffer);
                strcpy(buffer + i, name);
                buffer[i + 9] = fileInodeBlock;
                writeBlock(mounted_disk, 1, buffer);

                // write new inode block to disk
                buffer[0] = 0x02;
                buffer[1] = 0x44;
                buffer[2] = 0;
                writeBlock(mounted_disk, fileInodeBlock, buffer);
            }
        }
        fd_num++;
        addFileToOFT(name, fd_num, 0, fileInodeBlock, 0);
        return fd_num;
    }
}

int tfs_closeFile(fileDescriptor FD) {
    // Check if a file system is mounted
    if (mounted_disk < 0) {
        return ERROR_NO_FILE_SYSTEM_MOUNTED;
    }

    // Check if the file descriptor is valid
    if (FD < 0 || FD > fd_num) {
        return ERROR_INVALID_FILE_DESCRIPTOR;
    }

    // Remove the file from the OFT
    TfsFile *file = findFileFDInList(FD);
    if (file == NULL) {
        return ERROR_FILE_NOT_FOUND_IN_OFT;
    } else {
        removeFileFromOFT(FD);
    }
    
    return SUCCESS_TFS_CLOSE_FILE;
}

int tfs_writeFile(fileDescriptor FD, char *buffer, int size) {
    int totalblocks = (size / (BLOCKSIZE - 4));
    if (size % (BLOCKSIZE / 4) != 0) {
        totalblocks++;
    }

    TfsFile *file = findFileFDInList(FD);
    if (file == NULL) {
        return ERROR_FILE_NOT_OPEN;
    }

    // find free blocks and determine if enough space exists on disk before writing
    char dataBuffer[BLOCKSIZE];
    int freeBlock = -1;
    readBlock(mounted_disk, 0, dataBuffer);
    freeBlock = dataBuffer[2];
    for (int i = 0; i < totalblocks - 1; i++) {
        if (freeBlock == 0) {
            return ERROR_NOT_ENOUGH_FREE_BLOCKS;
        }
        readBlock(mounted_disk, freeBlock, dataBuffer);
        freeBlock = dataBuffer[2];
    }
    // enough space exists, write data to disk
    readBlock(mounted_disk, 0, dataBuffer);
    int newDataBlock = dataBuffer[2];
    for (int i = 0; i < totalblocks; i++) {
        readBlock(mounted_disk, newDataBlock, dataBuffer);
        dataBuffer[0] = 0x03;
        dataBuffer[1] = 0x44;
        dataBuffer[2] = 0;
        for (int j = 0; j < BLOCKSIZE - 4; j++) {
            dataBuffer[j + 4] = buffer[i * (BLOCKSIZE - 4) + j];
        }
        if (writeBlock(mounted_disk, newDataBlock, dataBuffer) < 0) {
            return ERROR_WRITING_DATA_TO_FILE_EXTENT_BLOCK_WRITEFILE;
        }

        // update the files inode block
        readBlock(mounted_disk, file->inodeBlock, dataBuffer);
        dataBuffer[4 + i] = newDataBlock;
        if (writeBlock(mounted_disk, file->inodeBlock, dataBuffer) < 0) {
            return ERROR_WRITING_DATA_TO_INODE_BLOCK_WRITEFILE;
        }
    }
    file->size += size;
    file->filePointer = 0;
    return SUCCESS_TFS_WRITE_FILE;
}

int tfs_deleteFile(fileDescriptor FD) {
    if (mounted_disk < 0) {
        return ERROR_NO_FILE_SYSTEM_MOUNTED;
    }

    TfsFile *file = findFileFDInList(FD);
    if (file == NULL) {
        return ERROR_FILE_NOT_FOUND_IN_OFT;
    } else {
        int inodeBlock = file->inodeBlock;
        char inodeBuffer[BLOCKSIZE];
        char buffer[BLOCKSIZE];
        readBlock(mounted_disk, inodeBlock, inodeBuffer);
        
        // for all data blocks in inode block, set byte 0 to 0x04 and byte 2 to point to next free block
        int i = 4;
        while (inodeBuffer[i] != 0) {
            if (inodeBuffer[i + 1] == 0) {  
                // check if superblock already has pointer to free block
                readBlock(mounted_disk, 0, buffer);
                int currFirstFreeBlock = buffer[2];
                if (buffer[2] == 0) {
                    buffer[2] = buffer[4];
                    writeBlock(mounted_disk, 0, buffer);
                                                    
                    memset(buffer, 0, BLOCKSIZE);   // clear data block
                    buffer[0] = 0x04;
                    buffer[1] = 0x44;
                    buffer[2] = inodeBlock;
                    writeBlock(mounted_disk, inodeBuffer[i], buffer);

                    memset(buffer, 0, BLOCKSIZE);   // clear inode block
                    buffer[0] = 0x04;
                    buffer[1] = 0x44;
                    writeBlock(mounted_disk, inodeBlock, buffer);
                }
                else {
                    buffer[2] = buffer[4];
                    writeBlock(mounted_disk, 0, buffer);
                                                    
                    memset(buffer, 0, BLOCKSIZE);   // clear data block
                    buffer[0] = 0x04;
                    buffer[1] = 0x44;
                    buffer[2] = inodeBlock;
                    writeBlock(mounted_disk, inodeBuffer[i], buffer);

                    memset(buffer, 0, BLOCKSIZE);   // clear inode block
                    buffer[0] = 0x04;
                    buffer[1] = 0x44;
                    buffer[2] = currFirstFreeBlock;
                    writeBlock(mounted_disk, inodeBlock, buffer);
                }
            }
        }
        
        // Remove the file from the OFT
        removeFileFromOFT(FD);
    }
    return SUCCESS_TFS_DELETE_FILE;
}

int tfs_readByte(fileDescriptor FD, char *buffer) {
    if (mounted_disk < 0) {
        return ERROR_NO_FILE_SYSTEM_MOUNTED;
    }

    TfsFile *file = findFileFDInList(FD);
    if (file == NULL) {
        return ERROR_FILE_NOT_FOUND_IN_OFT;
    }

    if (file->filePointer >= file->size) {
        return ERROR_READ_BYTE;
    }

    // Read the byte from the file
    int dataBlock = file->filePointer / (BLOCKSIZE - 4);
    int offset = file->filePointer % (BLOCKSIZE - 4);
    char dataBuffer[BLOCKSIZE];
    readBlock(mounted_disk, file->inodeBlock, dataBuffer);
    readBlock(mounted_disk, dataBuffer[4 + dataBlock], dataBuffer);
    *buffer = dataBuffer[offset + 4];
    file->filePointer++;

    return SUCCESS_TFS_READ_FILE;
}

int tfs_seek(fileDescriptor FD, int offset) {
    if (mounted_disk < 0) {
        return ERROR_NO_FILE_SYSTEM_MOUNTED;
    } else if (offset < 0) {
        return ERROR_TFS_SEEK_FILE;
    }
    
    TfsFile *file = findFileFDInList(FD);
    if (file == NULL) {
        return ERROR_FILE_NOT_FOUND_IN_OFT;
    } else if (offset > file->size) {
        return ERROR_TFS_SEEK_FILE;
    } else {
        file->filePointer = offset;
    }
    return SUCCESS_TFS_SEEK_FILE;
}

int tfs_rename(fileDescriptor FD, char *newName) {
    if (mounted_disk < 0) {
        return ERROR_NO_FILE_SYSTEM_MOUNTED;
    }

    TfsFile *file = findFileFDInList(FD);
    char* name = file->filename;
    if (file == NULL) {
        return ERROR_FILE_NOT_FOUND_IN_OFT;
    } else {
        // Rename the file
        strcpy(file->filename, newName);
        char buffer[BLOCKSIZE];
        readBlock(mounted_disk, 1, buffer);
        int i = 4;
        while (buffer[i] != 0) {
            if (strcmp(buffer + i, name) == 0) {
                break;
            }
            i++;
        }
        strcpy(buffer + i, newName);
        writeBlock(mounted_disk, 1, buffer);
    }
    return SUCCESS_TFS_RENAME_FILE;
}

int tfs_readdir() {
    if (mounted_disk < 0) {
        return ERROR_NO_FILE_SYSTEM_MOUNTED;
    }
    
    // Read the root directory
    char buffer[BLOCKSIZE];
    readBlock(mounted_disk, 1, buffer);

    // Print the file names in the root directory
    int i = 4;
    while (buffer[i] != 0) {
        printf("%s\n", buffer + i);
        i = i + 10;
    }
    return SUCCESS_TFS_READ_DIR;
}