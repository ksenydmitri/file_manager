#include "ui.h"
#include "dialog.h"
#include "../config/config.h"
#include "../ui/ui.h"
#include "../../include/types.h"
#include "../operations/file_ops.h"
#include "../errors/error.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>

static WINDOW* left_win;
static WINDOW* right_win;
static WINDOW* status_win;

void ui_init() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    ui_handle_resize();
}

void ui_cleanup() {
    delwin(left_win);
    delwin(right_win);
    delwin(status_win);
    endwin();
}

void ui_handle_resize() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    if(left_win) delwin(left_win);
    if(right_win) delwin(right_win);
    if(status_win) delwin(status_win);

    int panel_width = (max_x - 2) / 2;

    left_win = newwin(max_y - 2, panel_width, 1, 1);
    right_win = newwin(max_y - 2, panel_width, 1, panel_width + 1);
    status_win = newwin(1, max_x, max_y - 1, 0);

    refresh();
}

static void draw_panel(WINDOW* win, const Tab* tab, int is_active) {
    werase(win);

    int max_y;
    int max_x;
    getmaxyx(win, max_y, max_x);

    // Draw panel border differently if active
    if (is_active) {
        wattron(win, A_BOLD);
        box(win, 0, 0);
        wattroff(win, A_BOLD);
    } else {
        box(win, 0, 0);
    }

    // Header
    wattron(win, A_BOLD | COLOR_PAIR(1));
    mvwprintw(win, 0, 2, "%s", tab->path);
    wattroff(win, A_BOLD | COLOR_PAIR(1));

    for(int i = tab->offset; i < tab->file_count && i < tab->offset + max_y - 2; i++) {
        int y_pos;
        int attr;

        y_pos = i - tab->offset + 1;
        attr = 0;

        if(i == tab->selected) {
            attr |= A_REVERSE;
        }

        if(tab->files[i].type == FILE_DIRECTORY) {
            attr |= COLOR_PAIR(1);
        } else {
            attr |= COLOR_PAIR(2);
        }

        wattron(win, attr);
        mvwprintw(win, y_pos, 2, "%c %-30s %8ld",
                (tab->files[i].type == FILE_DIRECTORY) ? 'D' : 'F',
                tab->files[i].name,
                tab->files[i].size);
        wattroff(win, attr);
    }

    wrefresh(win);
}

void ui_draw_interface(const ApplicationState* state) {
    const Tab* left_tab = &state->tabs[0];
    const Tab* right_tab = &state->tabs[1];

    draw_panel(left_win, left_tab, state->active_tab == 0);
    draw_panel(right_win, right_tab, state->active_tab == 1);

    // Статус бар
    werase(status_win);
    wattron(status_win, A_BOLD);
    mvwprintw(status_win, 0, 0, " F1:Help  F3:Delete  F5:Copy  F6:Move  F10:Exit ");
    wattroff(status_win, A_BOLD);
    wrefresh(status_win);

    refresh();
}

// Загрузка содержимого директории во вкладку
void load_directory(Tab* tab) {
    DIR* dir = opendir(tab->path);
    if (!dir) {
        char error_msg[MAX_PATH_LEN + 50];
        snprintf(error_msg, sizeof(error_msg), "Cannot open directory %s", tab->path);
        error_handle(ERR_FILE_NOT_FOUND, __FILE__, __LINE__, error_msg);
        return;
    }

    struct dirent* entry;
    tab->file_count = 0;

    while ((entry = readdir(dir)) && tab->file_count < MAX_FILES_PER_DIR) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // Пропускаем текущий и родительский каталоги
        }

        // Заполняем структуру FileEntry
        FileEntry* file = &tab->files[tab->file_count++];
        char full_path[MAX_PATH_LEN];
        snprintf(full_path, MAX_PATH_LEN, "%s/%s", tab->path, entry->d_name);

        // Используем file_ops для извлечения данных о файле
        if (get_file_info(full_path, file) != 0) {
            tab->file_count--; // Если ошибка, не засчитываем файл
        }
    }

    closedir(dir);
}