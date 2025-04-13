#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <sys/types.h>
#include "../../include/enums.h"
#include "../../include/types.h"

#define MAX_PATH_LEN 4096
#define MAX_FILENAME_LEN 256

// Функции для работы с файлами и директориями
int file_create(const char* path);
int dir_create(const char* path);
int file_delete(const char* path);
int dir_delete_recursive(const char* path);
int delete_path(const char* path);
int file_copy(const char* src, const char* dest);
int file_move(const char* src, const char* dest);
int get_file_info(const char* path, FileEntry* entry);
void perform_file_search(const char* initial_directory, const char* target_name, FileSearchResult* results);

#endif // FILE_OPS_H
