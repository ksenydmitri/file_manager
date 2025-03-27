#include "path_utils.h"
#include "../../include/constants.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <libgen.h>

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