#include "ui.h"
#include <form.h>

void init_ui() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    init_pair(COLOR_DIR, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_FILE, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_LINK, COLOR_CYAN, COLOR_BLACK);
}

void draw_interface(Tab *tabs, int active_tab, const Clipboard *cb) {
    clear();
    Tab *t = &tabs[active_tab];

    // Рисуем вкладки
    for (int i = 0; i < MAX_TABS; i++) {
        if (i == active_tab) attron(A_REVERSE);
        mvprintw(0, i*15, " Tab %d ", i+1);
        attroff(A_REVERSE);
    }

    // Информационная строка
    attron(A_BOLD);
    mvprintw(1, 0, "Path: %s", t->path);
    attroff(A_BOLD);

    // Список файлов
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    for (int i = t->offset; i < t->file_count && i < t->offset + max_y - 5; i++) {
        int y = i - t->offset + 3;
        if (i == t->selected) attron(A_REVERSE);

        // Цвета для разных типов
        if (t->files[i].is_dir) {
            attron(COLOR_PAIR(COLOR_DIR));
        } else {
            attron(COLOR_PAIR(COLOR_FILE));
        }

        mvprintw(y, 0, "%c %-30s %10ld",
                t->files[i].is_dir ? 'D' : 'F',
                t->files[i].name,
                t->files[i].size);

        if (i == t->selected) attroff(A_REVERSE);
    }

    // Статус бар
    mvprintw(max_y-2, 0, "Clipboard: %s [%s]",
            cb->source,
            cb->type == OP_COPY ? "Copy" : "Move");

    mvprintw(max_y-1, 0, "F1-New F2-Rename F3-Delete F5-Copy F6-Move F10-Quit");

    refresh();
}

// Остальные функции интерфейса...
#include "ui.h"
