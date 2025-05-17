#include "path_utils.h"
#include "../../include/constants.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <stdio.h>

#include "../../include/types.h"

void normalize_path(char *path) {
    char temp[MAX_PATH_LEN];
    if (realpath(path, temp)){
        strncpy(path, temp, MAX_PATH_LEN);
    }
}

void get_parent_dir(char *path) {
    const char *parent = dirname(path);
    strncpy(path, parent, MAX_PATH_LEN);
}

int is_valid_path(const char *path) {
    return access(path, F_OK) == 0;
}

char* get_full_path(ApplicationState *state,const char *name) {
    char *full_path = malloc(MAX_PATH_LEN);
    if (!full_path) {
        perror("Memory allocation failed");
        return NULL;
    }
    snprintf(full_path, MAX_PATH_LEN, "%s/%s", state->tabs[state->active_tab].path,name);
    return full_path;
}

int validate_path(const char* path) {
    if (!path || strlen(path) >= MAX_PATH_LEN) {
        error_handle(ERR_INVALID_ARG, __FILE__, __LINE__, "Invalid path");
        return -1;
    }
    return 0;
}