#include "string_utils.h"
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>


void str_trim(char* str) {
    char *end = str + strlen(str) - 1;
    while (end >= str && isspace(*end)) end--;
    *(end + 1) = '\0';

    const char *start = str;
    while (*start && isspace(*start)) start++;

    memmove(str, start, end - start + 2);
}

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

void truncate_filename(char* dest, const char* src, size_t max_len) {
    if (strlen(src) > max_len) {
        strncpy(dest, src, max_len - 3);
        strcpy(dest + max_len - 3, "...");
    } else {
        strcpy(dest, src);
    }
}

char* format_time(time_t raw_time) {
    char* buffer = malloc(30);
    if (!buffer) return NULL;

    struct tm *time_info = localtime_r(&raw_time);
    strftime(buffer, 30, "%d %b %Y %H:%M", time_info);

    return buffer;
}

char* format_permissions(mode_t mode) {
    char* buffer = malloc(20);

    if (!buffer) return NULL;

    sprintf(buffer,"%c%c%c%c%c%c%c%c%c",
           (mode & S_IRUSR) ? 'r' : '-',
           (mode & S_IWUSR) ? 'w' : '-',
           (mode & S_IXUSR) ? 'x' : '-',
           (mode & S_IRGRP) ? 'r' : '-',
           (mode & S_IWGRP) ? 'w' : '-',
           (mode & S_IXGRP) ? 'x' : '-',
           (mode & S_IROTH) ? 'r' : '-',
           (mode & S_IWOTH) ? 'w' : '-',
           (mode & S_IXOTH) ? 'x' : '-');
    return buffer;
}

int calculate_width(int max_x) {
    const int max_width = 80;
    const int min_width = 10;
    const int padding = 4;

    if (max_x > max_width) {
        return max_width;
    }

    int reduced_width = max_x - padding;
    return (reduced_width > min_width) ? reduced_width : min_width;
}

int calculate_height(int max_y) {
    const int max_height = 20;
    const int min_height = 5;
    const int padding = 4;

    if (max_y > max_height) {
        return max_height;
    }

    int reduced_height = max_y - padding;
    return (reduced_height > min_height) ? reduced_height : min_height;
}
