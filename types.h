#ifndef TYPES_H
#define TYPES_H

#include <sys/stat.h>

#define MAX_PATH 4096
#define MAX_FILES 2048
#define MAX_TABS 5

typedef struct {
    char name[256];
    int is_dir;
    off_t size;
    mode_t mode;
} FileEntry;

typedef struct {
    char path[MAX_PATH];
    FileEntry files[MAX_FILES];
    int file_count;
    int selected;
    int offset;
} Tab;

typedef enum {
    OP_COPY,
    OP_MOVE
} OperationType;

typedef struct {
    char source[MAX_PATH];
    OperationType type;
} Clipboard;

#endif