#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>
#include <time.h>
#include <sys/types.h>

void str_trim(char* str);
void format_file_size(char* buffer, size_t size);
int is_hidden_file(const char* filename);
void str_copy(char* dest, const char* src, size_t max_len);
void truncate_filename(char* dest, const char* src, size_t max_len);
char *format_time(time_t raw_time);
char* format_permissions(mode_t mode);

#endif