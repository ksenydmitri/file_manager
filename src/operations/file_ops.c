#include "file_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include "error.h"

int file_create(const char* path) {
    FILE* fp = fopen(path, "w");
    if (!fp) return -1;
    fclose(fp);
    return 0;
}

int dir_create(const char* path) {
    return mkdir(path, 0755);
}

int file_delete(const char* path) {
    if (unlink(path) == -1) {
        error_handle(ERR_IO_FAILURE, __FILE__, __LINE__,
                   "Failed to delete: %s", path);
        return -1;
    }
    return 0;
}

int file_copy(const char* src, const char* dest) {
    FILE *source = fopen(src, "rb");
    if (!source) return -1;

    FILE *destination = fopen(dest, "wb");
    if (!destination) {
        fclose(source);
        return -1;
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) {
        fwrite(buffer, 1, bytes, destination);
    }

    fclose(source);
    fclose(destination);
    return 0;
}

int file_move(const char* src, const char* dest) {
    return rename(src, dest);
}

int get_file_info(const char* path, FileEntry* entry) {
    struct stat st;
    if (lstat(path, &st) return -1;

    entry->is_dir = S_ISDIR(st.st_mode);
    entry->size = st.st_size;
    entry->mode = st.st_mode;
    return 0;
}

int dir_delete_recursive(const char* path) {
    DIR *dir = opendir(path);
    if (!dir) return -1;

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;

        char full_path[MAX_PATH];
        path_join(full_path, path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            dir_delete_recursive(full_path);
        } else {
            unlink(full_path);
        }
    }
    closedir(dir);
    return rmdir(path);
}