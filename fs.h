#ifndef MN_FS
#define MN_FS
#include <errno.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "error.h"

ErrorCode
getContentsPosix(char const  *filename, char **contents, size_t *length);

ErrorCode
getContentsRegularFile(
    const char  *filename
    , struct stat  *stat_buf
    , int fd
    , char **contents
    , size_t *length
);
#endif
