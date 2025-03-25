#include "ops.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int create_file(const char *path) {
    int fd = creat(path, 0644);
    if (fd == -1) return -1;
    close(fd);
    return 0;
}

int create_dir(const char *path) {
    return mkdir(path, 0755);
}

int delete_file(const char *path) {
    FileEntry entry;
    if (get_file_info(path, &entry) return -1;
    
    if (entry.is_dir) return rmdir(path);
    return unlink(path);
}

int rename_file(const char *old_path, const char *new_path) {
    return rename(old_path, new_path);
}

void show_properties(const FileEntry *entry) {
    // Реализация диалога с информацией
    // ...
}