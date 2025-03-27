#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>

// Обрезка пробелов
void str_trim(char* str);

// Форматирование размера файла
void format_file_size(char* buffer, size_t size);

// Проверка на скрытый файл
int is_hidden_file(const char* filename);

// Безопасное копирование строки
void str_copy(char* dest, const char* src, size_t max_len);

#endif