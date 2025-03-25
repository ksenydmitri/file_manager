#ifndef OPS_H
#define OPS_H

#include "file_utils.h"

int create_file(const char *path);
int create_dir(const char *path);
int delete_file(const char *path);
int rename_file(const char *old_path, const char *new_path);
void show_properties(const FileEntry *entry);

#endif //OPS_H
