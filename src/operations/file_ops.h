#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "../include/types.h"
#include "../utils/path_utils.h"

// Создание файла
int file_create(const char* path);

// Создание директории
int dir_create(const char* path);

// Удаление файла/директории
int file_delete(const char* path);

// Копирование файла/директории
int file_copy(const char* src, const char* dest);

// Перемещение файла/директории
int file_move(const char* src, const char* dest);

// Получение информации о файле
int get_file_info(const char* path, FileEntry* entry);

// Рекурсивное удаление директории
int dir_delete_recursive(const char* path);


int load_directory(Tab* tab);

#endif