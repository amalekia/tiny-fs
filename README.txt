Adrick Malekian

Emulating TinyFS (the tiny file system), mounted on a single Unix file that emulates a block device

libDisk:

The libDisk.c file contains functions and global variables related to emulating the TinyFS file system on a Unix file that acts as a block device.
The functions in libDisk.c are responsible for managing the disk blocks and performing operations on them. These functions include:

int openDisk(char *filename, int nBytes): Opens the disk file with the specified filename and sets its size to nBytes. 
Returns a file descriptor for the opened disk file.

int readBlock(int disk, int bNum, void *block): Reads the block with the specified block number bNum from the disk with the given file descriptor 
disk into the memory location pointed to by block. Returns 0 on success, or -1 if an error occurs.

int writeBlock(int disk, int bNum, void *block): Writes the block pointed to by block to the disk with the specified file descriptor disk at the
block number bNum. Returns 0 on success, or -1 if an error occurs.

int closeDisk(int disk): Closes the disk file associated with the given file descriptor disk. Returns 0 on success, or -1 if an error occurs.

tinyFs:

limitations - bitmap for free blocks can only hold 252 * 8 bits of free blocks
            - 

