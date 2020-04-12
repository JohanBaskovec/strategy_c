#include "fs.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


ErrorCode
getContentsPosix(char const *filename, char **contents , size_t *length) {
    struct stat stat_buf;
    int fd;

    fd = open(filename, O_RDONLY);

    if (fd < 0) {
        return MN_FILE_OPEN_FAILED;
    }

    if (fstat (fd, &stat_buf) < 0) {
        close (fd);

        return MN_FILE_FSSTTAT_FAILED;
    }

    if (stat_buf.st_size > 0 && S_ISREG (stat_buf.st_mode)) {
        return getContentsRegularFile(
            filename
            , &stat_buf
            , fd
            , contents
            , length
        );
    } else {
        // not implemented
        return MN_FILE_NOT_A_REGULAR_FILE;
    }
    return MN_NO_ERROR;
}

ErrorCode
getContentsRegularFile(
    const char  *filename
    , struct stat  *stat_buf
    , int fd
    , char **contents
    , size_t *length
) {
    ErrorCode ret = MN_NO_ERROR;

    size_t size = stat_buf->st_size;

    size_t alloc_size = size + 1;
    char *buf = malloc(alloc_size);

    if (buf == NULL) {
        ret = MN_OUT_OF_MEMORY;
        goto end;
    }

    size_t bytes_read = 0;
    while (bytes_read < size) {
        size_t rc = read (fd, buf + bytes_read, size - bytes_read);

        if (rc < 0) {
            if (errno != EINTR) {
                ret = MN_FILE_READ_FAILED;
                goto end;
            }
        } else if (rc == 0) {
            break;
        } else {
            bytes_read += rc;
        }
    }

    buf[bytes_read] = '\0';

    if (length) {
        *length = bytes_read;
    }

    *contents = buf;

end:
    close (fd);
    return ret;
}
