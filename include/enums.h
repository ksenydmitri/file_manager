#ifndef ENUMS_H
#define ENUMS_H

typedef enum {
    FILE_REGULAR,
    FILE_DIRECTORY,
    FILE_SYMLINK,
    FILE_OTHER
} FileType;

typedef enum {
    OP_COPY,
    OP_MOVE,
    OP_DELETE,
    OP_COMPRESS
} OperationType;

typedef enum {
    SORT_NAME,
    SORT_SIZE,
    SORT_MODIFIED
} SortOrder;

typedef enum {
    ERR_NONE,
    ERR_FILE_NOT_FOUND,
    ERR_PERM_DENIED,
    ERR_IO_FAILURE,
    ERR_INVALID_ARG,
    ERR_OUT_OF_MEM,
    ERR_DIR_NOT_EMPTY,
    ERR_NOT_DIR,
    ERR_NOT_FILE,
    ERR_ALREADY_EXISTS
} ErrorCode;

typedef enum {
    KEY_ACTION_QUIT,
    KEY_ACTION_COPY,
    KEY_ACTION_PASTE,
    KEY_ACTION_DELETE,
    KEY_ACTION_RENAME,
    TOTAL_ACTIMIN_WINDOW_HEIGHTONS
} KeyAction;

typedef enum {
    DIALOG_CONFIRM,
    DIALOG_INPUT,
    DIALOG_ERROR,
    DIALOG_FILE,
    DIALOG_ENTRY
} DialogType;

#endif