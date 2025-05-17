#include "../operations/file_ops.h"
#include "../operations/clipboard.h"
#include "../ui/ui.h"
#include "../ui/dialog.h"
#include <ncurses.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include "../../include/constants.h"
#include "../errors/error.h"
#include "input.h"

#include <path_utils.h>
#include <stdlib.h>
#include <unistd.h>

void navigate_up(ApplicationState* state);
void navigate_down(ApplicationState* state);
void go_back(ApplicationState* state);
void delete_selected(ApplicationState* state);
void handle_copy_operation(ApplicationState* state);
void handle_paste_operation(ApplicationState* state);
void handle_create_operation(ApplicationState* state);
void enter_file_or_directory(ApplicationState* state);
void handle_rename(ApplicationState* state);
void handle_change_permissions(ApplicationState* state);
void handle_change_owner(ApplicationState* state);
void handle_file_entry(ApplicationState* state);
void handle_search_anomaly(ApplicationState* state);
void handle_check_system_stat(ApplicationState* state);
void handle_misc(ApplicationState* state);

void handle_actions(ApplicationState* state, int key) {
    switch (key) {
        case 10:
            enter_file_or_directory(state);
            break;
        case KEY_BACKSPACE:
            go_back(state);
            break;
        case KEY_F(1):
            handle_create_operation(state);
            break;
        case KEY_F(2):
            handle_change_owner(state);
            break;
        case KEY_F(3):
            delete_selected(state);
            break;
        case KEY_F(4):
            handle_file_entry(state);
            break;
        case KEY_F(5):
            handle_copy_operation(state);
            break;
        case KEY_F(6):
            handle_paste_operation(state);
            break;
        case KEY_F(7):
            handle_change_permissions(state);
            break;
        case KEY_F(8):
            handle_rename(state);
            break;
        case 9:
            handle_search_anomaly(state);
            break;
        case 's':
            handle_check_system_stat(state);
            break;
        case KEY_F(9):
            state->should_exit = 1;
            break;
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
            state->active_tab = (state->active_tab < MAX_TABS - 1) ? state->active_tab + 1 : MAX_TABS - 1;
        break;
        case 'p':
            handle_misc(state);
        break;
        default:
        break;
    }
}

void handle_misc(ApplicationState* state) {
        show_search_dialog(state);
}

void handle_rename(ApplicationState* state) {
    show_rename_dialog(state, &state->tabs[state->active_tab]
        .files[state->tabs[state->active_tab].selected]);
}

void handle_input(ApplicationState* state, int key) {
    handle_actions(state, key);
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

void enter_directory(ApplicationState* state,Tab* tab, const FileEntry* entry) {
  	char *new_path = get_full_path(state,entry->name);
    strncpy(tab->path, new_path, MAX_PATH_LEN);
    load_directory(tab);
    tab->selected = 0;
    tab->offset = 0;
    free(new_path);
}

void enter_file(const Tab* tab,const FileEntry* entry) {
 	show_file_dialog(tab->path, entry);
}

void enter_symlink(ApplicationState* state,const FileEntry* entry) {
    char *new_path = get_full_path(state,entry->name);
    int fd = open(new_path,O_RDONLY);
    if(fd == -1) {
        show_error_dialog("Can't open symlink");
    } else {
        char target[1024];
        ssize_t len = readlink(new_path, target, sizeof(target) - 1);
        if (len != -1) {
            show_file_contents_dialog(target);
        } else {
            show_error_dialog("Can't read symlink");
        }
    }
    free(new_path);
}

void enter_file_or_directory(ApplicationState* state) {
  	Tab* tab = &state->tabs[state->active_tab];
    const FileEntry* entry = &tab->files[tab->selected];
    switch(entry->type) {
      case FILE_DIRECTORY:
            enter_directory(state, tab, entry);
            break;
      case FILE_REGULAR:
            enter_file(tab, entry);
            break;
      case FILE_SYMLINK:
            enter_symlink(state, entry);
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

void handle_change_owner(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    FileEntry* entry = &tab->files[tab->selected];
    if (!show_change_owner_dialog(state, entry)) {
        show_error_dialog("Can't change owner");
        return;
    }
}

void handle_create_operation(ApplicationState* state) {
    show_create_object_dialog(state);
}

void handle_copy_operation(ApplicationState* state) {
    const Tab* tab = &state->tabs[state->active_tab];
    if (tab->selected < tab->file_count) {
        const FileEntry* entry = &tab->files[tab->selected];
        char *full_path = get_full_path(state,entry->name);
        copy_to_clipboard(&state->clipboard, full_path, OP_COPY);
        free(full_path);
    }
}

void handle_paste_operation(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    if (strlen(state->clipboard.source) > 0) {
        paste_from_clipboard(&state->clipboard, tab->path);
        load_directory(tab);
    }
}

void handle_change_permissions(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    FileEntry* entry = &tab->files[tab->selected];
    if (entry->type == FILE_REGULAR ) {
        show_change_permissions_dialog(state, entry);
    } else {
        show_error_dialog("Selected object not file!");
        return;
    }
}

void handle_file_entry(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    FileEntry* entry = &tab->files[tab->selected];
    if (entry->type == FILE_REGULAR ) {
        show_file_entry_dialog(entry);
    } else {
        show_error_dialog("Selected object not file!");
        return;
    }
}

void handle_search_anomaly(ApplicationState* state) {
    const Tab* tab = &state->tabs[state->active_tab];
    iterate_filesystem(tab->path);
    show_file_contents_dialog(get_full_path(state, "results.txt"));
}

void handle_check_system_stat(ApplicationState* state) {
    show_system_stat_dialog(state);
}

void delete_selected(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    if (tab->selected >= tab->file_count) return;

    const FileEntry* entry = &tab->files[tab->selected];
    char *path = get_full_path(state,entry->name);

    if (delete_path(path) != 0) {
        char error_msg[MAX_PATH_LEN + 50];
        snprintf(error_msg, sizeof(error_msg), "Failed to delete: %s", path);
        error_handle(ERR_IO_FAILURE, __FILE__, __LINE__, error_msg);
        return;
    }

    load_directory(tab);
    free(path);
}