#include "../operations/file_ops.h"
#include "../operations/clipboard.h"
#include "../ui/ui.h"
#include "../ui/dialog.h"
#include <ncurses.h>
#include <signal.h>
#include <string.h>
#include "../../include/constants.h"
#include "../errors/error.h"
#include "input.h"
#include "../operations/file_ops.h"

void navigate_up(ApplicationState* state);
void navigate_down(ApplicationState* state);
void go_back(ApplicationState* state);
void delete_selected(ApplicationState* state);
void handle_copy_operation(ApplicationState* state);
void handle_paste_operation(ApplicationState* state);
void handle_create_operation(ApplicationState* state);
void enter_file_or_directory(ApplicationState* state);

void handle_input(ApplicationState* state, int key) {

    switch(key) {
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

        case 10:
            enter_file_or_directory(state);
        break;

        case KEY_BACKSPACE:
            go_back(state);
        break;

        case KEY_F(1):
            show_create_object_dialog(state);
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
        case 'p':
            show_search_dialog(state);
            break;

        default:
       	break;
    }
    state->need_refresh = 1;
}

void navigate_up(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    if(tab->selected > 0) {
        tab->selected--;
        if(tab->selected < tab->offset) {
            tab->offset = tab->selected;
        }
    }

}

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

void enter_directory(ApplicationState* state, Tab* tab, FileEntry* entry) {
  	char new_path[MAX_PATH_LEN];
    snprintf(new_path, MAX_PATH_LEN, "%s/%s", tab->path, entry->name);
    strncpy(tab->path, new_path, MAX_PATH_LEN);
    load_directory(tab);
    tab->selected = 0;
    tab->offset = 0;
}

void enter_file(ApplicationState* state, Tab* tab, FileEntry* entry) {
 	show_file_dialog(state, tab->path, entry);
}

void enter_file_or_directory(ApplicationState* state) {
  	Tab* tab = &state->tabs[state->active_tab];
    FileEntry* entry = &tab->files[tab->selected];
    switch(entry->type) {
      case FILE_DIRECTORY:
        enter_directory(state, tab, entry);
        break;
      case FILE_REGULAR:
        enter_file(state, tab, entry);
        break;
      case FILE_SYMLINK:
        break;
      case FILE_OTHER:
        break;
    }
}

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
        load_directory(tab);
    }
}

void delete_selected(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    if (tab->selected >= tab->file_count) return;

    FileEntry* entry = &tab->files[tab->selected];
    char path[MAX_PATH_LEN];
    snprintf(path, MAX_PATH_LEN, "%s/%s", tab->path, entry->name);

    if (delete_path(path) != 0) {
        char error_msg[MAX_PATH_LEN + 50];
        snprintf(error_msg, sizeof(error_msg), "Failed to delete: %s", path);
        error_handle(ERR_IO_FAILURE, __FILE__, __LINE__, error_msg);
        return;
    }

    load_directory(tab);
}