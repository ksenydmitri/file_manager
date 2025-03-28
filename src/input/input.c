#include "../operations/file_ops.h"
#include "../operations/clipboard.h"
#include "../ui/ui.h"
#include <ncurses.h>
#include <signal.h>
#include <string.h>
#include "../../include/constants.h"
#include "input.h"

void handle_input(ApplicationState* state, int key) {
    // Убрали неиспользуемую переменную current_tab

    switch(key) {
        // Навигация
        case KEY_UP:
            navigate_up(state);
        break;

        case KEY_DOWN:
            navigate_down(state);
        break;

        case KEY_LEFT:
            state->active_tab = (state->active_tab > 0) ? state->active_tab - 1 : 0;
        break;

        case KEY_RIGHT:
            state->active_tab = (state->active_tab < MAX_TABS-1) ? state->active_tab + 1 : MAX_TABS-1;
        break;

        // Действия с файлами
        case 10: // Enter
            enter_directory(state);
        break;

        case KEY_BACKSPACE:
            go_back(state);
        break;

        case KEY_F(3):
            delete_selected(state);
        break;

        case KEY_F(5):
            handle_copy_operation(state);
        break;

        case KEY_F(6):
            handle_paste_operation(state);
        break;

        case KEY_F(10):
            state->should_exit = 1;
        break;

        default:
            // Можно добавить обработку неизвестных клавиш
                break;
    }
}

// Навигация вверх
void navigate_up(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    if(tab->selected > 0) {
        tab->selected--;
        if(tab->selected < tab->offset) {
            tab->offset = tab->selected;
        }
    }
}

// Навигация вниз
void navigate_down(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    if(tab->selected < tab->file_count - 1) {
        tab->selected++;
        int max_visible = getmaxy(stdscr) - 5;
        if(tab->selected >= tab->offset + max_visible) {
            tab->offset++;
        }
    }
}

// Вход в директорию
void enter_directory(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    FileEntry* entry = &tab->files[tab->selected];
    
    if(entry->type == FILE_DIRECTORY) {
        char new_path[MAX_PATH_LEN];
        snprintf(new_path, MAX_PATH_LEN, "%s/%s", tab->path, entry->name);
        strncpy(tab->path, new_path, MAX_PATH_LEN);
        load_directory(tab);
        tab->selected = 0;
        tab->offset = 0;
    }
}

// Возврат на уровень выше
void go_back(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    char* last_slash = strrchr(tab->path, '/');
    
    if(last_slash != NULL) {
        *last_slash = '\0';
        if(strlen(tab->path) == 0) strcpy(tab->path, "/");
        load_directory(tab);
        tab->selected = 0;
        tab->offset = 0;
    }
}

// Реализация новых функций в input.c
void handle_copy_operation(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    if (tab->selected < tab->file_count) {
        FileEntry* entry = &tab->files[tab->selected];
        char full_path[MAX_PATH_LEN];
        snprintf(full_path, MAX_PATH_LEN, "%s/%s", tab->path, entry->name);
        copy_to_clipboard(&state->clipboard, full_path, OP_COPY);
    }
}

void handle_paste_operation(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    if (strlen(state->clipboard.source) > 0) {
        paste_from_clipboard(&state->clipboard, tab->path);
        load_directory(tab); // Обновить содержимое директории
    }
}