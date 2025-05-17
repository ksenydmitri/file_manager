#ifndef PATH_UTILS_H
#define PATH_UTILS_H
#include <types.h>

void normalize_path(char *path);
void get_parent_dir(char *path);
int is_valid_path(const char *path);
char* get_full_path(ApplicationState *state,const char *name);
int validate_path(const char* path);

#endif
