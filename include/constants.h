#ifndef CONSTANTS_H
#define CONSTANTS_H

// Лимиты системы
#define MAX_PATH_LEN 4096
#define MAX_FILENAME_LEN 256
#define MAX_FILES_PER_DIR 4096
#define MAX_TABS 5
#define MAX_HISTORY 100
#define MAX_DIRECTORIES 10000

// Значения по умолчанию
#define DEFAULT_TERM_WIDTH 80
#define DEFAULT_TERM_HEIGHT 24
#define INITIAL_DIR "."

// Цветовые пары
#define COLOR_PAIR_DIR 1
#define COLOR_PAIR_FILE 2
#define COLOR_PAIR_LINK 3
#define COLOR_PAIR_SELECTED 4

#define KEY_ESCAPE 27
#define KEY_ENTER 10

#define MAX_SEARCH_RESULTS 100
#define SECTOR_SIZE 512

#define MIN_WINDOW_HEIGHT 10
#define MIN_WINDOW_WIDTH 10

#endif