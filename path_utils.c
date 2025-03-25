#include "path_utils.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

void normalize_path(char *path) {
    char temp[PATH_MAX];
    if (realpath(path, temp) {
        strncpy(path, temp, PATH_MAX);
    }
}

void get_parent_dir(char *path) {
    char *parent = dirname(path);
    strncpy(path, parent, PATH_MAX);
}

int is_valid_path(const char *path) {
    return access(path, F_OK) == 0;
}