#include "error.h"
#include "../../include/constants.h"
#include "../ui/ui.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

static ErrorInfo last_error;

void error_init() {
    memset(&last_error, 0, sizeof(ErrorInfo));
}

void error_handle(ErrorCode code, const char* file, int line, const char* message) {
    last_error.code = code;
    last_error.line = line;

    strncpy(last_error.file, file, MAX_PATH_LEN-1);
    last_error.file[MAX_PATH_LEN-1] = '\0';

    strncpy(last_error.message, message, sizeof(last_error.message)-1);
    last_error.message[sizeof(last_error.message)-1] = '\0';
}

void error_show_last() {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), 
        "[%s:%d] %s: %s",
        last_error.file, 
        last_error.line,
        error_code_to_string(last_error.code),
        last_error.message);
    
    show_error_dialog(buffer);
}

const char* error_code_to_string(ErrorCode code) {
    static const char* strings[] = {
        [ERR_NONE] = "No error",
        [ERR_FILE_NOT_FOUND] = "File not found",
        [ERR_PERM_DENIED] = "Permission denied",
        [ERR_IO_FAILURE] = "I/O failure",
        [ERR_INVALID_ARG] = "Invalid argument",
        [ERR_OUT_OF_MEM] = "Out of memory",
        [ERR_DIR_NOT_EMPTY] = "Directory not empty",
        [ERR_NOT_DIR] = "Not a directory",
        [ERR_NOT_FILE] = "Not a regular file",
        [ERR_ALREADY_EXISTS] = "Already exists"
    };
    return strings[code];
}