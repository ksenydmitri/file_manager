#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>  // Для off_t, mode_t, uid_t, gid_t
#include <time.h>       // Для time_t
#include "enums.h"      // Для FileType, ColorScheme
#include "../src/config/config.h"
#include "constants.h"

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

typedef struct {
    Tab tabs[MAX_TABS];
    Clipboard clipboard;
    AppConfig config;
    int active_tab;
    int should_exit;
    int need_refresh;
} ApplicationState;

// Результат диалога
typedef struct {
    int confirmed;
    char input[256];
} DialogResult;

typedef struct {
    char name[MAX_FILENAME_LEN];
    char path[MAX_PATH_LEN];
} SearchResult;

typedef struct {
    SearchResult files[MAX_SEARCH_RESULTS];
    int count;
} FileSearchResult;

#endif