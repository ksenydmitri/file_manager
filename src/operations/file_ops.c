#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "file_ops.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mntent.h>
#include <dirent.h>
#include <string.h>
#include <dirent.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sys/statvfs.h>
#include "../errors/error.h"
#include "../utils/string_utils.h"
#include "../utils/path_utils.h"

static FileInfo processed_files[MAX_SEARCH_RESULTS];
static int processed_count = 0;

int file_create(const char* path) {
    FILE* fp = fopen(path, "w");
    if (!fp) return -1;
    fclose(fp);
    return 0;
}

int dir_create(const char* path) {
    return mkdir(path, 0755);
}

int delete_path(const char* path) {
    struct stat st;

    if (lstat(path, &st) != 0) {
        char error_msg[MAX_PATH_LEN + 50];
        snprintf(error_msg, sizeof(error_msg), "Failed to get file info: %s", path);
        error_handle(ERR_IO_FAILURE, __FILE__, __LINE__, error_msg);
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
        return dir_delete_recursive(path);
    } else {
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

    if (ret == 0 && rmdir(path) != 0) {
        char error_msg[MAX_PATH_LEN + 50];
        snprintf(error_msg, sizeof(error_msg), "Failed to delete directory: %s", path);
        error_handle(ERR_IO_FAILURE, __FILE__, __LINE__, error_msg);
        return -1;
    }

    return ret;
}

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

int get_file_info(const char* path, FileEntry* entry) {
    struct stat st;
    if (lstat(path, &st) != 0) {
        return -1;
    }

    if (S_ISREG(st.st_mode)) {
        entry->type = FILE_REGULAR;
    } else if (S_ISDIR(st.st_mode)) {
        entry->type = FILE_DIRECTORY;
    } else if (S_ISLNK(st.st_mode)) {
        entry->type = FILE_SYMLINK;
    } else {
        entry->type = FILE_OTHER;
    }

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

void search_files(const char* directory, const char* query, FileSearchResult* results) {
    DIR* dir = opendir(directory);
    if (!dir) {
        fprintf(stderr, "Ошибка открытия директории %s: %s\n", directory, strerror(errno));
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && results->count < MAX_SEARCH_RESULTS) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char full_path[MAX_PATH_LEN];
        snprintf(full_path, MAX_PATH_LEN, "%s/%s", directory, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0) {
            fprintf(stderr, "Ошибка получения информации о файле %s: %s\n", full_path, strerror(errno));
            continue;
        }

        if (S_ISREG(st.st_mode) && strstr(entry->d_name, query)) {
            strncpy(results->files[results->count].name, entry->d_name, MAX_FILENAME_LEN - 1);
            strncpy(results->files[results->count].path, full_path, MAX_PATH_LEN - 1);
            results->files[results->count].name[MAX_FILENAME_LEN - 1] = '\0';
            results->files[results->count].path[MAX_PATH_LEN - 1] = '\0';
            results->count++;
        }
    }

    closedir(dir);
}

void perform_iterative_search(const char* root_directory, const char* query, FileSearchResult* results) {
    char directories[MAX_SEARCH_RESULTS][MAX_PATH_LEN];
    int dir_count = 0;

    strncpy(directories[dir_count++], root_directory, MAX_PATH_LEN - 1);

    while (dir_count > 0 && results->count < MAX_SEARCH_RESULTS) {
        char current_directory[MAX_PATH_LEN];
        strncpy(current_directory, directories[--dir_count], MAX_PATH_LEN - 1);

        DIR* dir = opendir(current_directory);
        if (!dir) continue;

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            char *full_path[MAX_PATH_LEN];
            snprintf(full_path, MAX_PATH_LEN, "%s/%s", current_directory, entry->d_name);

            struct stat st;
            if (stat(full_path, &st) != 0) continue;

            if (S_ISREG(st.st_mode) && strstr(entry->d_name, query)) {
                strncpy(results->files[results->count].name, entry->d_name, MAX_FILENAME_LEN - 1);
                strncpy(results->files[results->count].path, full_path, MAX_PATH_LEN - 1);
                results->files[results->count].name[MAX_FILENAME_LEN - 1] = '\0';
                results->files[results->count].path[MAX_PATH_LEN - 1] = '\0';
                results->count++;
            }

            if (S_ISDIR(st.st_mode) && dir_count < MAX_SEARCH_RESULTS) {
                strncpy(directories[dir_count++], full_path, MAX_PATH_LEN - 1);
            }
        }

        closedir(dir);
    }
}

int is_duplicate(FileSearchResult* results, const char* path) {
    if (!path || results->count == 0) return 0;
    const char* filename = strrchr(path, '/');
    filename = (filename != NULL) ? filename + 1 : path;
    for (int i = 0; i < results->count; i++) {
        if (!results->files[i].path) continue;

        const char* existing_filename = strrchr(results->files[i].path, '/');
        existing_filename = (existing_filename != NULL) ? existing_filename + 1 : results->files[i].path;

        if (strcmp(filename, existing_filename) == 0) {
            return 1;
        }
    }
    return 0;
}



void perform_file_search(const char* initial_directory, const char* target_name, FileSearchResult* results) {
    results->count = 0;
    search_files(initial_directory, target_name, results);
    perform_iterative_search(initial_directory, target_name, results);
    FileSearchResult filtered_results = {0};
    for (int i = 0; i < results->count; i++) {
        if (!is_duplicate(&filtered_results, results->files[i].path)) {
            filtered_results.files[filtered_results.count++] = results->files[i];
        }
    }
    *results = filtered_results;
}

int rename_file(const char* old_name, const char* new_name) {
    if (rename(old_name, new_name) == 0) {
        return 0;
    }
    show_error_dialog("Can't rename file");
    return -1;
}

int change_permissions(const char* path, mode_t mode) {
    if (chmod(path, mode) == 0) {
        return 0;
    }
    show_error_dialog("Can't change permissions");
    return -1;
}

int change_owner(const char* path, uid_t owner, gid_t group) {
    if (chown(path, owner, group) == 0) {
        return 0;
    }
    show_error_dialog("Can't change owner");
    return -1;
}

void process_file(const char* path,FILE* file) {
    struct stat st;
    if (lstat(path, &st) == -1) {
        show_error_dialog( "Error accessing file");
        return;
    }

    int is_broken_link = 0;
    if (S_ISLNK(st.st_mode)) {
        if (stat(path, &st) == -1) {
            is_broken_link = 1;
            fprintf(file, "Broken symlink found: %s\n", path);
        }
    }

    int is_duplicate = 0;
    for (int i = 0; i < processed_count; i++) {
        if (processed_files[i].inode == st.st_ino) {
            fprintf(file, "Duplicate file found (same inode %lu):\n  %s\n  %s\n",
                   (unsigned long)st.st_ino, processed_files[i].path, path);
            is_duplicate = 1;
        }

        if (strcmp(processed_files[i].path, path) == 0) {
            fprintf(file, "Duplicate path found: %s\n", path);
            is_duplicate = 1;
        }
    }

    time_t now = time(NULL);
    time_t unused_threshold = 30 * 24 * 60 * 60;

    if ((now - st.st_atime) > unused_threshold) {
        fprintf(file,"Potentially unused file (last accessed %ld days ago): %s\n",
              (now - st.st_atime) / (24 * 60 * 60), path);
    }

    if (processed_count < MAX_SEARCH_RESULTS && !is_duplicate) {
        strncpy(processed_files[processed_count].path, path, MAX_PATH_LEN - 1);
        processed_files[processed_count].path[MAX_PATH_LEN - 1] = '\0';
        processed_files[processed_count].inode = st.st_ino;
        processed_files[processed_count].last_access = st.st_atime;
        processed_files[processed_count].is_broken_link = is_broken_link;
        processed_count++;
    }
}


void iterate_filesystem(const char* root_dir) {
    FILE* file = fopen("result.txt", "w");
    if (file == NULL) {
        show_error_dialog("Can't open results file");
    }
    DIR* dir;
    struct dirent* entry;

    char* directories[MAX_DIRECTORIES];
    int top = 0;

    directories[top++] = strdup(root_dir);

    while (top > 0) {
        char* current_dir = directories[--top];

        dir = opendir(current_dir);
        if (!dir) {
            free(current_dir);
            continue;
        }

        while ((entry = readdir(dir))) {
            char path[MAX_PATH_LEN];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            snprintf(path, MAX_PATH_LEN, "%s/%s", current_dir, entry->d_name);

            struct stat st;
            if (stat(path, &st) != 0) continue;

            if (S_ISDIR(st.st_mode)) {
                if (top < MAX_DIRECTORIES) directories[top++] = strdup(path);
            } else {
                process_file(path,file);
            }
        }
        closedir(dir);
        free(current_dir);
    }
    fclose(file);
}

struct mntent* get_file_system_mntent() {
    FILE *mtab = setmntent("/etc/mtab", "r");

    struct mntent *entry = getmntent(mtab);
    endmntent(mtab);
    return entry;
}

DiskStats get_disk_stats(const char* device) {
    FILE* diskstats = fopen("/proc/diskstats", "r");
    DiskStats stats = {0};
    char line[256];

    while (fgets(line, sizeof(line), diskstats)) {
        if (strstr(line, device)) {
            sscanf(line, "%*u %*u %*s %lu %*u %lu %lu %*u %lu",
                   &stats.reads, &stats.read_sectors,
                   &stats.writes, &stats.write_sectors);
            break;
        }
    }
    fclose(diskstats);
    return stats;
}

void find_root_device(char* device) {
    DIR *dir = opendir("/sys/block");
    struct dirent *entry;

    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_LNK &&
            !strstr(entry->d_name, "loop") &&
            !strstr(entry->d_name, "dm-")) {
            strcpy(device, entry->d_name);
            break;
            }
    }
    closedir(dir);
}

void measure_io_speed(const char* device,SystemInfo* space_info) {
    DiskStats stats1, stats2;
    double time_interval = 1.0;

    stats1 = get_disk_stats(device);
    clock_t start = clock();

    sleep(time_interval);

    stats2 = get_disk_stats(device);
    clock_t end = clock();

    double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    unsigned long read_diff = stats2.read_sectors - stats1.read_sectors;
    unsigned long write_diff = stats2.write_sectors - stats1.write_sectors;

    space_info->reed_speed = (read_diff * SECTOR_SIZE) / (1024.0 * 1024.0 * elapsed);
    space_info->write_speed = (write_diff * SECTOR_SIZE) / (1024.0 * 1024.0 * elapsed);
}

SystemInfo* check_disk_stat(const char* path) {
    SystemInfo* space_info = malloc(sizeof(SystemInfo));
    struct statvfs fs_info;

    if (statvfs(path, &fs_info) == -1) {
        show_error_dialog("Ошибка statvfs");
        free(space_info);
        return NULL;
    }

    const unsigned long block_size = fs_info.f_frsize;
    const unsigned long total_blocks = fs_info.f_blocks;
    const unsigned long free_blocks = fs_info.f_bfree;
    const unsigned long available_blocks = fs_info.f_bavail;

    space_info->total_space = (double)(total_blocks * block_size) / (1024 * 1024 * 1024);
    space_info->free_space = (double)(free_blocks * block_size) / (1024 * 1024 * 1024);
    space_info->available_space = (double)(available_blocks * block_size) / (1024 * 1024 * 1024);

    space_info->entry = get_file_system_mntent();
    char device[32] = {0};
    find_root_device(device);
    measure_io_speed(device,space_info);
    return space_info;
}
