#include "file_utils.h"
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int load_directory(Tab *tab) {
    DIR *dir = opendir(tab->path);
    if (!dir) return -1;

    struct dirent *entry;
    tab->file_count = 0;

    if (strcmp(tab->path, "/") != 0) {
        strcpy(tab->files[0].name, "..");
        tab->files[0].is_dir = 1;
        tab->file_count = 1;
    }

    while ((entry = readdir(dir))) {
        if (entry->d_name[0] == '.') continue;
        
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", tab->path, entry->d_name);
        
        FileEntry *fe = &tab->files[tab->file_count];
        if (get_file_info(full_path, fe) == 0) {
            strncpy(fe->name, entry->d_name, 255);
            tab->file_count++;
        }
        
        if (tab->file_count >= MAX_FILES) break;
    }
    closedir(dir);
    return 0;
}

int get_file_info(const char *path, FileEntry *entry) {
    struct stat st;
    if (lstat(path, &st)) return -1;
    
    entry->is_dir = S_ISDIR(st.st_mode);
    entry->size = st.st_size;
    entry->mode = st.st_mode;
    return 0;
}