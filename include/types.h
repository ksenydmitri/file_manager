#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>  // Для off_t, mode_t, uid_t, gid_t
#include <time.h>       // Для time_t
#include "enums.h"      // Для FileType, ColorScheme
#include "config.h"     // Для AppConfig

#define MAX_FILENAME_LEN 256
#define MAX_PATH_LEN 4096
#define MAX_FILES_PER_DIR 4096

typedef struct {
    char name[MAX_FILENAME_LEN];
    off_t size;
    mode_t mode;
    time_t mtime;
    uid_t uid;
    gid_t gid;
    FileType type;
} FileEntry;

typedef struct {
    char source[MAX_PATH_LEN];
    OperationType op_type;
    FileType content_type;
} Clipboard;

typedef struct {
    char path[MAX_PATH_LEN];
    FileEntry files[MAX_FILES_PER_DIR];
    int file_count;
    int selected;
    int offset;
    SortOrder sort_order;
} Tab;

#endif