#include "ui.h"
#include "ops.h"
#include <unistd.h>

int main() {
    Tab tabs[MAX_TABS] = {0};
    Clipboard cb = {0};
    int active_tab = 0;
    int ch;

    // Инициализация
    for (int i = 0; i < MAX_TABS; i++) {
        getcwd(tabs[i].path, MAX_PATH);
        load_directory(&tabs[i]);
    }

    init_ui();

    // Главный цикл
    while ((ch = getch()) != KEY_F(10)) {
        Tab *t = &tabs[active_tab];

        switch(ch) {
            case KEY_LEFT:
                if (active_tab > 0) active_tab--;
            break;
            case KEY_RIGHT:
                if (active_tab < MAX_TABS-1) active_tab++;
            break;
            case KEY_UP:
                if (t->selected > 0) t->selected--;
            break;
            case KEY_DOWN:
                if (t->selected < t->file_count-1) t->selected++;
            break;
            case '\n':
                // Обработка входа в директорию
                    break;
            case KEY_F(1):
                // Создание файла/директории
                    break;
            // Обработка других клавиш...
        }

        draw_interface(tabs, active_tab, &cb);
    }

    endwin();
    return 0;
}
