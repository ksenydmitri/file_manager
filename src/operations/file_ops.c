#define _POSIX_C_SOURCE 200809L
#include "file_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include "../errors/error.h"

int file_create(const char* path) {
    FILE* fp = fopen(path, "w");
    if (!fp) return -1;
    fclose(fp);
    return 0;
}

int dir_create(const char* path) {
    return mkdir(path, 0755);
}

int file_delete(const char* path) {
    if (unlink(path) == -1) {
        error_handle(ERR_IO_FAILURE, __FILE__, __LINE__,
                   "Failed to delete: %s", path);
        return -1;
    }
    return 0;
}

int file_copy(const char* src, const char* dest) {
    FILE *source = fopen(src, "rb");
    if (!source) return -1;

    FILE *destination = fopen(dest, "wb");
    if (!destination) {
        fclose(source);
        return -1;
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source))) {
        fwrite(buffer, 1, bytes, destination);
    }

    fclose(source);
    fclose(destination);
    return 0;
}

int file_move(const char* src, const char* dest) {
    return rename(src, dest);
}

int get_file_info(const char* path, FileEntry* entry) {
    struct stat st;
    if (lstat(path, &st) != 0) {
        return -1;  // Ошибка при получении информации о файле
    }

    // Определение типа файла
    if (S_ISREG(st.st_mode)) {
        entry->type = FILE_REGULAR;
    } else if (S_ISDIR(st.st_mode)) {
        entry->type = FILE_DIRECTORY;
    } else if (S_ISLNK(st.st_mode)) {
        entry->type = FILE_SYMLINK;
    } else {
        entry->type = FILE_OTHER;
    }

    // Заполнение структуры FileEntry
    const char* filename = strrchr(path, '/');
    filename = (filename != NULL) ? filename + 1 : path;
    strncpy(entry->name, filename, MAX_FILENAME_LEN - 1);
    entry->name[MAX_FILENAME_LEN - 1] = '\0';  // Гарантируем завершающий нуль

    entry->size = st.st_size;
    entry->mode = st.st_mode;
    entry->mtime = st.st_mtime;
    entry->uid = st.st_uid;
    entry->gid = st.st_gid;

    return 0;  // Успешное выполнение
}

// Вспомогательная функция для объединения путей
static void path_join(char* dest, const char* dir, const char* file) {
    snprintf(dest, MAX_PATH_LEN, "%s/%s", dir, file);
}

int dir_delete_recursive(const char* path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir failed");
        return -1;
    }

    struct dirent *entry;
    int ret = 0;

    while ((entry = readdir(dir)) != NULL && ret == 0) {
        // Пропускаем "." и ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[MAX_PATH_LEN];
        path_join(full_path, path, entry->d_name);

        // Обработка для символических ссылок (чтобы не следовать по ним)
        struct stat st;
        if (lstat(full_path, &st) == -1) {
            perror("lstat failed");
            ret = -1;
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            // Рекурсивное удаление поддиректории
            if (dir_delete_recursive(full_path) != 0) {
                ret = -1;
            }
        } else {
            // Удаление файла
            if (unlink(full_path) != 0) {
                perror("unlink failed");
                ret = -1;
            }
        }
    }

    closedir(dir);

    // Удаляем саму директорию, если не было ошибок
    if (ret == 0 && rmdir(path) != 0) {
        perror("rmdir failed");
        ret = -1;
    }

    return ret;
}

void load_directory(Tab* tab) {
    DIR* dir = opendir(tab->path);
    if (!dir) {
        error_handle(ERR_FILE_NOT_FOUND, __FILE__, __LINE__, "Cannot open directory %s", tab->path);
        return;
    }

    struct dirent* entry;
    tab->file_count = 0;

    while ((entry = readdir(dir)) && tab->file_count < MAX_FILES_PER_DIR) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        FileEntry* file = &tab->files[tab->file_count++];
        get_file_info(entry->d_name, file);
    }
    closedir(dir);
}

void delete_selected(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    if (tab->selected >= tab->file_count) return;

    FileEntry* entry = &tab->files[tab->selected];
    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s", tab->path, entry->name);

    if (entry->type == FILE_DIRECTORY) {
        if (rmdir(path) != 0) {
            error_handle(ERR_IO_FAILURE, __FILE__, __LINE__, "Failed to delete directory");
            return;
        }
    } else {
        if (unlink(path) != 0) {
            error_handle(ERR_IO_FAILURE, __FILE__, __LINE__, "Failed to delete file");
            return;
        }
    }

    load_directory(tab); // Обновляем список файлов
}