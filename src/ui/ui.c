#include "ui.h"
#include "dialog.h"
#include "../config/config.h"
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

    // Заголовок
    wattron(win, A_BOLD | COLOR_PAIR(1));
    mvwprintw(win, 0, 2, "%s", tab->path);
    wattroff(win, A_BOLD | COLOR_PAIR(1));

    // Список файлов
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);

    for(int i = tab->offset;
        i < tab->file_count && i < tab->offset + max_y - 2;
        i++) {

        int y_pos = i - tab->offset + 1;

        if(i == tab->selected) {
            wattron(win, A_REVERSE);
        }

        // Тип файла
        if(tab->files[i].is_dir) {
            wattron(win, COLOR_PAIR(1));
            mvwaddch(win, y_pos, 2, 'D');
        } else {
            wattron(win, COLOR_PAIR(2));
            mvwaddch(win, y_pos, 2, 'F');
        }

        // Имя и размер
        mvwprintw(win, y_pos, 5, "%-30s %8ld",
                tab->files[i].name,
                tab->files[i].size);

        if(i == tab->selected) {
            wattroff(win, A_REVERSE);
        }
    }

    box(win, 0, 0);
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