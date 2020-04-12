#ifndef ST_ERROR
#define ST_ERROR

#define fatalError(f, ...) \
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, f, ## __VA_ARGS__);\
    exit(1);

typedef enum ErrorCode {
    MN_NO_ERROR
    , MN_FILE_OPEN_FAILED
    , MN_FILE_READ_FAILED
    , MN_FILE_FSSTTAT_FAILED
    , MN_FILE_FDOPEN_FAILED
    , MN_OUT_OF_MEMORY
    , MN_FILE_NOT_A_REGULAR_FILE
} ErrorCode;

static char const *errorString[] = {
    "MN_NO_ERROR"
    , "MN_FILE_OPEN_FAILED"
    , "MN_FILE_READ_FAILED"
    , "MN_FILE_FSSTTAT_FAILED"
    , "MN_FILE_FDOPEN_FAILED"
    , "MN_OUT_OF_MEMORY"
    , "MN_FILE_NOT_A_REGULAR_FILE"
};

static inline char const * errorGetString(ErrorCode code) {
    return errorString[code];
}

void
fatalSdlError(const char* funcName);

#endif
