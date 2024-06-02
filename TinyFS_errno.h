// Description: Error codes for tinyFS functions

// Error codes for libDisk functions
#define FILE_OPEN_ERROR -1
#define FILE_CLOSE_ERROR -2
#define FILE_READ_ERROR -3
#define FILE_WRITE_ERROR -4
#define FILE_SEEK_ERROR -5
#define ERROR_FILE_NAME_TOO_LONG -6
#define ERROR_DISK_NOT_FOUND -7

// Error codes for tinyFS functions
#define ERROR_INVALID_FILE_SYSTEM -8
#define ERROR_NO_FILE_SYSTEM_MOUNTED -9

// Sucessful return values for libDisk
#define FILE_OPEN_SUCCESS 0
#define FILE_CLOSE_SUCCESS 0
#define FILE_READ_SUCCESS 0
#define FILE_WRITE_SUCCESS 0
#define FILE_SEEK_SUCCESS 0

// Successful return values for tinyFS
#define SUCCESS_TFS_DISK_CREATED 0
#define SUCCESS_TFS_DISK_MOUNTED 0
#define SUCCESS_TFS_DISK_UNMOUNTED 0 
#define SUCCESS_TFS_OPEN_FILE 0
#define SUCCESS_TFS_CLOSE_FILE 0
#define SUCCESS_TFS_WRITE_FILE 0
#define SUCCESS_TFS_DELETE_FILE 0
#define SUCCESS_TFS_READ_FILE 0
#define SUCCESS_TFS_SEEK_FILE 0