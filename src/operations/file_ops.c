#define _POSIX_C_SOURCE 200809L
#include "file_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include "../errors/error.h"

// ======== Создание файлов и директорий ======== //
int file_create(const char* path) {
    FILE* fp = fopen(path, "w");
    if (!fp) return -1;
    fclose(fp);
    return 0;
}

int dir_create(const char* path) {
    return mkdir(path, 0755);
}

// ======== Удаление файла или директории ======== //
int delete_path(const char* path) {
    struct stat st;

    // Получение информации о файле/директории
    if (lstat(path, &st) != 0) {
        char error_msg[MAX_PATH_LEN + 50];
        snprintf(error_msg, sizeof(error_msg), "Failed to get file info: %s", path);
        error_handle(ERR_IO_FAILURE, __FILE__, __LINE__, error_msg);
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
        // Удаляем директорию рекурсивно
        return dir_delete_recursive(path);
    } else {
        // Удаляем файл
        return file_delete(path);
    }
}

int file_delete(const char* path) {
    if (unlink(path) == -1) {
        char error_msg[MAX_PATH_LEN + 50];
        snprintf(error_msg, sizeof(error_msg), "Failed to delete file: %s", path);
        error_handle(ERR_IO_FAILURE, __FILE__, __LINE__, error_msg);
        return -1;
    }
    return 0;
}

int dir_delete_recursive(const char* path) {
    DIR* dir = opendir(path);
    if (!dir) {
        char error_msg[MAX_PATH_LEN + 50];
        snprintf(error_msg, sizeof(error_msg), "Cannot open directory: %s", path);
        error_handle(ERR_IO_FAILURE, __FILE__, __LINE__, error_msg);
        return -1;
    }

    struct dirent* entry;
    int ret = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // Пропуск "." и ".."
        }

        char full_path[MAX_PATH_LEN];
        snprintf(full_path, MAX_PATH_LEN, "%s/%s", path, entry->d_name);

        if (delete_path(full_path) != 0) {
            ret = -1; // Запоминаем ошибку, но продолжаем обработку остальных файлов
        }
    }

    closedir(dir);

    // Удаляем директорию
    if (ret == 0 && rmdir(path) != 0) {
        char error_msg[MAX_PATH_LEN + 50];
        snprintf(error_msg, sizeof(error_msg), "Failed to delete directory: %s", path);
        error_handle(ERR_IO_FAILURE, __FILE__, __LINE__, error_msg);
        return -1;
    }

    return ret;
}

// ======== Копирование и перемещение файлов ======== //
int file_copy(const char* src, const char* dest) {
    FILE* source = fopen(src, "rb");
    if (!source) return -1;

    FILE* destination = fopen(dest, "wb");
    if (!destination) {
        fclose(source);
        return -1;
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, destination);
    }

    fclose(source);
    fclose(destination);
    return 0;
}

int file_move(const char* src, const char* dest) {
    return rename(src, dest);
}

// ======== Получение информации о файле ======== //
int get_file_info(const char* path, FileEntry* entry) {
    struct stat st;
    if (lstat(path, &st) != 0) {
        return -1;
    }

    // Определяем тип файла
    if (S_ISREG(st.st_mode)) {
        entry->type = FILE_REGULAR;
    } else if (S_ISDIR(st.st_mode)) {
        entry->type = FILE_DIRECTORY;
    } else if (S_ISLNK(st.st_mode)) {
        entry->type = FILE_SYMLINK;
    } else {
        entry->type = FILE_OTHER;
    }

    // Инициализация имени файла
    const char* filename = strrchr(path, '/');
    filename = (filename != NULL) ? filename + 1 : path;
    strncpy(entry->name, filename, MAX_FILENAME_LEN - 1);
    entry->name[MAX_FILENAME_LEN - 1] = '\0';

    entry->size = st.st_size;
    entry->mode = st.st_mode;
    entry->mtime = st.st_mtime;
    entry->uid = st.st_uid;
    entry->gid = st.st_gid;

    return 0;
}
