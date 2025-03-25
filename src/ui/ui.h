#ifndef UI_H
#define UI_H

#include <ncurses.h>
#include "utils.h"

#define COLOR_DIR 1
#define COLOR_FILE 2
#define COLOR_LINK 3

void init_ui();
void draw_interface(Tab *tabs, int active_tab, const Clipboard *cb);
void show_context_menu(int x, int y);
void show_search_dialog(char *buffer);

#endif

#endif //UI_H
