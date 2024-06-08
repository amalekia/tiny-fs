Adrick Malekian

Emulating TinyFS (the tiny file system), mounted on a single Unix file that emulates a block device

Implementation of tinyFS:

First I developed the libDisk library which takes care of handling all of the neccesary read and writes to the disk. This
program utilizes a linked list of open disks to keep track of what disks are being modified and how so. All functions in 
this library look back on the linked list to refer to items such as the FILE*, size of disk, and the name assoicated with the disk.
Aside from that the function are very trivial and take care of the basic read and write operations to a disk as well as opening and
closing a disk along with deallocating any resources tied to a file.

For the libTinyFS, we can see the main user callable functions. These include making a file system, opening a file system,
mounting and unmounting a file system, writing to a file in the file system, closing a file in the file system, deleting a file in the 
file system, seeking in a file in the file system, and reading a byte from a file in the file system. The extra functionalities I added were
file renaming and readdir as well as read only and writeByte. Some limitations and drawbacks that are worth mentioning are the fact that 
in the inode block we can see some forms of internal fragmentation. For one, I designed my project to have a strict size of 10 bytes per inode
and 9 of those bytes are for the filename (including null terminator). Since a filename can be shorter than 8 bytes, we lose some space in each
inode entry that doesnt use the full 8 bytes for filename. Also since we are going by multiples of 10 bytes per entry, we would be able to reach
up to byte 254 (max entries). This leaves 2 bytes to go to waste which isnt alot but its something worth noting.


tinyFs:

limitations - internal fragmentation when writing filename to inode blocks
            - having a root directory of size 252 bytes limits the amount of files someone can have

bugs - writeByte doesnt write to the correct spot

