#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "types.h"
#include "path_utils.h"

int load_directory(Tab *tab);
int get_file_info(const char *path, FileEntry *entry);
off_t calculate_dir_size(const char *path);

#endif
