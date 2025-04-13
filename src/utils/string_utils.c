#include "string_utils.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

void str_trim(char* str) {
    char *end = str + strlen(str) - 1;
    while (end >= str && isspace(*end)) end--;
    *(end + 1) = '\0';

    const char *start = str;  // Changed to pointer-to-const
    while (*start && isspace(*start)) start++;

    memmove(str, start, end - start + 2);
}

#include <stdio.h>

void format_file_size(char* buffer, size_t size) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    size_t unit = 0;
    double s = size;
    while (s >= 1024 && unit < 3) {
        s /= 1024;
        unit++;
    }

    if (unit == 0) {
        snprintf(buffer, 32, "%zu %s", size, units[unit]);
    } else {
        snprintf(buffer, 32, "%.2f %s", s, units[unit]);
    }
}


int is_hidden_file(const char* filename) {
    return filename[0] == '.';
}

void str_copy(char* dest, const char* src, size_t max_len) {
    strncpy(dest, src, max_len - 1);
    dest[max_len - 1] = '\0';
}