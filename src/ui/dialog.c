#include "dialog.h"
#include <ncurses.h>
#include <panel.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <ui.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <mntent.h>
#include <path_utils.h>

#include "../../include/types.h"
#include "../utils/string_utils.h"
#include "../operations/file_ops.h"

void print_truncated(WINDOW* win, int y, int x, const char* text, int max_width) {
    char truncated_text[max_width + 1];
    truncate_filename(truncated_text, text, max_width - 1);
    mvwprintw(win, y, x, "%s", truncated_text);
}

void print_wrapped_text(WINDOW* win, int start_y, int start_x, const char* text, int max_width) {
    int line = start_y;
    int current_x = start_x;
    int text_len = strlen(text);

    for (int i = 0; i < text_len; ) {
        int remaining_width = max_width - current_x;
        int chunk_len = (remaining_width < text_len - i) ? remaining_width : text_len - i;

        mvwprintw(win, line, current_x, "%.*s", chunk_len, &text[i]);

        i += chunk_len;
        line++;
        current_x = start_x;
    }
}

static void handle_input_dialog(WINDOW* win, int height, DialogResult* result) {
    echo();
    curs_set(1);

    print_truncated(win, height-3, 2, "Input: ", 8);
    wmove(win, height-3, 9);

    const int input_size = 255;
    wgetnstr(win, result->input, input_size);

    curs_set(0);
    noecho();
    result->confirmed = 1;
}

static void handle_entry_dialog(WINDOW *win,int height,DialogResult *result) {

}

static void handle_error_dialog(WINDOW* win, int height) {
    print_truncated(win, height-2, 2, "Press any key...", 20);
    wrefresh(win);
    wgetch(win);
}

static void handle_confirm_dialog(WINDOW* win, int height, DialogResult* result) {
    print_truncated(win, height-2, 10, "Yes (Y) / No (N)", 20);
    wrefresh(win);

    int done = 0;
    while (!done) {
        const int ch = wgetch(win);
        switch (tolower(ch)) {
            case 'y':
                result->confirmed = 1;
                result->input[0] = 'y';
                done = 1;
                break;
            case 'n':
                result->confirmed = 0;
                result->input[0] = 'n';
                done = 1;
                break;
            default:
                break;
        }
    }
}

DialogResult show_dialog(DialogType type, const char* title, const char* message) {
    DialogResult result = {0};

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int width = max_x > 50 ? 50 : max_x - 4;
    int height = max_y > 9 ? 9 : max_y - 4;

    const int start_x = (max_x - width) / 2;
    const int start_y = (max_y - height) / 2;

    WINDOW* dialog_win = newwin(height, width, start_y, start_x);
    if (!dialog_win) {
        return result;
    }

    PANEL* dialog_panel = new_panel(dialog_win);

    keypad(dialog_win, TRUE);
    box(dialog_win, 0, 0);

    wattron(dialog_win, A_BOLD);
    print_truncated(dialog_win, 0, 2, title, width - 4);
    wattroff(dialog_win, A_BOLD);

    print_truncated(dialog_win, 2, 2, message, width - 4);

    update_panels();
    doupdate();

    switch (type) {
        case DIALOG_CONFIRM:
            handle_confirm_dialog(dialog_win, height, &result);
            break;
        case DIALOG_INPUT:
            handle_input_dialog(dialog_win, height, &result);
            break;
        case DIALOG_ERROR:
            handle_error_dialog(dialog_win, height);
            break;
        case DIALOG_ENTRY:
            handle_entry_dialog(dialog_win, height, &result);
        default:
            break;
    }

    del_panel(dialog_panel);
    delwin(dialog_win);

    return result;
}

void show_file_contents_dialog(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        show_error_dialog("Failed to open file.");
        return;
    }

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int width = max_x > 80 ? 80 : max_x - 4;;
    int height = max_y > 20 ? 20 : max_y - 4;
    int padding = 2;

    const int start_x = (max_x - width) / 2;
    const int start_y = (max_y - height) / 2;

    WINDOW* dialog_win = newwin(height, width, start_y, start_x);
    if (!dialog_win) {
        fclose(file);
        show_error_dialog("Failed to create dialog window.");
        return;
    }

    keypad(dialog_win, TRUE);
    box(dialog_win, 0, 0);
    print_truncated(dialog_win, 0, 2, filepath, width - 4);

    werase(dialog_win);

    char buffer[256];
    int line = 1;
    int ch;
    int scroll_offset = 0;
    int total_lines = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        total_lines++;
    }
    rewind(file);

    while (1) {
        werase(dialog_win);
        box(dialog_win, 0, 0);
        wrefresh(dialog_win);
        print_truncated(dialog_win, 0, 2, filepath, width - 6);

        line = 1;
        int current_line = 0;
        while (fgets(buffer, sizeof(buffer), file)) {
            if (current_line >= scroll_offset && current_line < scroll_offset + height - padding) {
                print_wrapped_text(dialog_win, line++, 2, buffer, width - 8);
            }
            current_line++;
        }
        wrefresh(dialog_win);
        rewind(file);

        ch = wgetch(dialog_win);
        if (ch == 'q' || ch == 27) {
            break;
        } else if (ch == KEY_DOWN && scroll_offset < total_lines - (height - padding)) {
            scroll_offset++;
        } else if (ch == KEY_UP && scroll_offset > 0) {
            scroll_offset--;
        }
    }

    fclose(file);
    delwin(dialog_win);
}

void show_file_dialog(ApplicationState* state, const char* path, FileEntry* entry) {
    const char *full_path = get_full_path(state->tabs[state->active_tab].path, entry->name);
    show_file_contents_dialog(full_path);
    free((void*)full_path);
}

void show_error_dialog(const char* message) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int width = max_x > 60 ? 60 : max_x - 4;
    int height = 5;

    WINDOW* win = newwin(height, width, (max_y - height) / 2, (max_x - width) / 2);
    box(win, 0, 0);

    print_truncated(win, 1, 2, "Error:", width - 4);
    print_truncated(win, 2, 2, message, width - 4);
    print_truncated(win, 3, 2, "Press any key to continue...", width - 4);

    wrefresh(win);
    wgetch(win);
    delwin(win);
}


void show_create_object_dialog(ApplicationState* state) {
    DialogResult result = show_dialog(DIALOG_INPUT, "Create Object", "Create File (F) or Directory (D)?");

    if (!result.confirmed) return;

    char choice = tolower(result.input[0]);
    Tab* tab = &state->tabs[state->active_tab];

    switch (choice) {
        case 'f': {
            DialogResult name_result = show_dialog(DIALOG_INPUT, "Create File", "Enter file name:");
            if (!name_result.confirmed || strlen(name_result.input) == 0) {
                show_error_dialog("Name cannot be empty.");
                return;
            }

            char* full_path = get_full_path(state, name_result.input);

            if (file_create(full_path) != 0) {
                show_error_dialog("Failed to create the file.");
                free(full_path);
                return;
            }

            load_directory(tab);
            show_dialog(DIALOG_ERROR, "Success", "File created successfully!");
            free(full_path);
            break;
        }

        case 'd': {
            DialogResult dir_name_result = show_dialog(DIALOG_INPUT, "Create Directory", "Enter directory name:");
            if (!dir_name_result.confirmed || strlen(dir_name_result.input) == 0) {
                show_error_dialog("Name cannot be empty.");
                return;
            }

            char* full_path = get_full_path(state, dir_name_result.input);

            if (dir_create(full_path) != 0) {
                show_error_dialog("Failed to create the directory.");
                free(full_path);
                return;
            }

            load_directory(tab);
            show_dialog(DIALOG_ERROR, "Success", "Directory created successfully!");
            free(full_path);
            break;
        }

        default:
            show_error_dialog("Invalid choice. Press 'F' for File or 'D' for Directory.");
        break;
    }
}

void show_search_result_dialog(ApplicationState* state, FileSearchResult* results) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int width = max_x > 80 ? 80 : max_x - 4;;
    int height = max_y > 20 ? 20 : max_y - 4;

    const int start_x = (max_x - width) / 2;
    const int start_y = (max_y - height) / 2;

    WINDOW* dialog_win = newwin(height, width, start_y, start_x);
    if (!dialog_win) {
        show_error_dialog("Failed to create dialog window.");
        return;
    }

    keypad(dialog_win, TRUE);
    box(dialog_win, 0, 0);
    int line = 2;
    for (int i = 0; i < results->count; i++) {
        char truncated_path[width - 4];
        truncate_filename(truncated_path, results->files[i].path, width - 4);
        mvwprintw(dialog_win, line++, 2, "Path: %s", truncated_path);
    }
    wrefresh(dialog_win);
    wgetch(dialog_win);
    delwin(dialog_win);
}

void show_search_dialog(ApplicationState* state) {
    DialogResult result = show_dialog(DIALOG_INPUT, "Search Files", "Enter file name to search:");

    if (!result.confirmed || strlen(result.input) == 0) {
        return;
    }

    FileSearchResult search_results;
    perform_file_search(state->tabs[state->active_tab].path, result.input, &search_results);

    if (search_results.count == 0) {
        show_error_dialog("File not found.");
    } else {
        show_search_result_dialog(state,&search_results);
    }

    refresh();
}

void show_rename_dialog(ApplicationState* state, FileEntry* entry) {
    DialogResult result = show_dialog(DIALOG_INPUT, "Rename File", "Enter new file name:");

    if (!result.confirmed || strlen(result.input) == 0) {
        show_error_dialog("Name cannot be empty.");
        return;
    }

    const char *old_path = get_full_path(state, entry->name);
    const char *new_path = get_full_path(state, result.input);

    if (rename_file(old_path, new_path) == 0) {
        show_dialog(DIALOG_ERROR, "Success", "File renamed successfully!");
        load_directory(&state->tabs[state->active_tab]);
    } else {
        show_error_dialog("Failed to rename file.");
    }
    free((void*)old_path);
    free((void*)new_path);
}

void show_change_permissions_dialog(ApplicationState* state, FileEntry* entry) {
    const char *full_path = get_full_path(state->tabs[state->active_tab].path, entry->name);

    DialogResult result = show_dialog(DIALOG_INPUT, "Change Permissions", "Enter permission (e.g., 755):");

    if (!result.confirmed || strlen(result.input) == 0) {
        show_error_dialog("Permission cannot be empty.");
        free((void*)full_path);
        return;
    }

    mode_t new_mode = strtol(result.input, NULL, 8);

    if (chmod(full_path, new_mode) != 0) {
        show_error_dialog("Failed to change permissions.");
        free((void*)full_path);
    } else {
        show_dialog(DIALOG_ERROR, "Success", "Permissions updated successfully!");
        free((void*)full_path);
    }
}

int show_change_owner_dialog(ApplicationState *state, FileEntry *entry) {
    const char *full_path = get_full_path(state->tabs[state->active_tab].path, entry->name);

    DialogResult result_user = show_dialog(DIALOG_INPUT, "Change Owner", "Enter new owner username:");
    if (!result_user.confirmed || strlen(result_user.input) == 0) {
        show_error_dialog("Owner cannot be empty.");
        free((void*)full_path);
        return 1;
    }

    DialogResult result_group = show_dialog(DIALOG_INPUT, "Change Group", "Enter new group name:");
    if (!result_group.confirmed || strlen(result_group.input) == 0) {
        show_error_dialog("Group cannot be empty.");
        free((void*)full_path);
        return 1;
    }

    struct passwd *pwd = getpwnam(result_user.input);
    struct group *grp = getgrnam(result_group.input);

    if (!pwd || !grp) {
        show_error_dialog("Invalid username or group name.");
        free((void*)full_path);
        return 1;
    }

    if (chown(full_path, pwd->pw_uid, grp->gr_gid) != 0) {
        show_error_dialog("Failed to change owner.");
        free((void*)full_path);
        return 1;
    } else {
        show_dialog(DIALOG_ERROR, "Success", "Owner and group updated successfully!");
        free((void*)full_path);
        return 0;
    }
}//НЕ ЗАБЫТЬ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void show_file_entry_dialog(ApplicationState *state, FileEntry* file_entry) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int width = max_x > 80 ? 80 : max_x - 4;;
    int height = 12;

    const int start_x = (max_x - width) / 2;
    const int start_y = (max_y - height) / 2;

    WINDOW* win = newwin(height, width, start_y, start_x);
    if (!win) {
        show_error_dialog("Failed to create dialog window.");
        return;
    }

    keypad(win, TRUE);
    box(win, 0, 0);
    int line = 2;
    print_truncated(win,line++, 2,file_entry->name,width);
    switch (file_entry->type) {
        case FILE_REGULAR:
            print_truncated(win,line++, 2,"type: F",width);
        break;
        case FILE_DIRECTORY:
            print_truncated(win,line++, 2,"type: D",width);
        break;
        case FILE_SYMLINK:
            print_truncated(win,line++, 2,"type: S",width);
        break;
        case FILE_OTHER:
            print_truncated(win,line++, 2,"type: O",width);
        break;
        default:
            print_truncated(win,line++, 2,"type: ?",width);
        break;
    }
    char gid_str[32];
    char uid_str[32];
    char size_str[32];
    char mode_str[32];
    char mtime_str[32];

    snprintf(gid_str, sizeof(gid_str), "gid: %u", file_entry->gid);
    snprintf(uid_str, sizeof(uid_str), "uid: %u", file_entry->uid);
    snprintf(size_str, sizeof(size_str), "size: %ld", file_entry->size);
    char *formatted_time = format_time(file_entry->mtime);
    if (formatted_time) {
        snprintf(mtime_str, sizeof(mtime_str), "mtime: %s", formatted_time);
        free(formatted_time);
    } else {
        snprintf(mtime_str, sizeof(mtime_str), "mtime: error");
    }
    char *formatted_mode = format_permissions(file_entry->mode);
    if (formatted_mode) {
        snprintf(mode_str, sizeof(mtime_str), "mode: %s", formatted_mode);
        free(formatted_mode);
    } else {
        snprintf(mtime_str, sizeof(mtime_str), "mtime: error");
    }

    print_truncated(win, line++, 2, gid_str, width);
    print_truncated(win, line++, 2, uid_str, width);
    print_truncated(win, line++, 2, size_str, width);
    print_truncated(win, line++, 2, mode_str, width);
    print_truncated(win, line, 2, mtime_str, width);

    print_truncated(win, height-2, 2, "Press any key...", width);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

void show_system_stat_dialog(ApplicationState *state) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int width = max_x > 80 ? 80 : max_x - 4;;
    int height = 12;

    const int start_x = (max_x - width) / 2;
    const int start_y = (max_y - height) / 2;

    WINDOW* win = newwin(height, width, start_y, start_x);
    if (!win) {
        show_error_dialog("Failed to create dialog window.");
        return;
    }

    keypad(win, TRUE);
    box(win, 0, 0);
    int line = 2;

    SystemInfo *space_info = check_disk_stat(state->tabs[state->active_tab].path);

    char *total_space_buffer[32];
    char *free_space_buffer[32];
    char *available_space_buffer [64];
    char *reed_speed_buffer [64];
    char *write_speed_buffer [64];
    char *system_type_buffer [32];
    snprintf(total_space_buffer,sizeof(total_space_buffer),
        "Total space: %.2f GB", space_info->total_space);
    snprintf(free_space_buffer,sizeof(free_space_buffer),
        "Free space: %.2f GB", space_info->free_space);
    snprintf(available_space_buffer,sizeof(available_space_buffer),
        "Using for use: %.2f GB",space_info->available_space);
    snprintf(reed_speed_buffer,sizeof(reed_speed_buffer),
        "Reed speed : %.2f MB/s",space_info->reed_speed);
    snprintf(write_speed_buffer,sizeof(write_speed_buffer),
        "Write speed : %.2f MB/s",  space_info->write_speed);
    snprintf(system_type_buffer,sizeof(system_type_buffer),"Type FS: %s", space_info->entry->mnt_type);

    print_truncated(win, line++, 2, total_space_buffer, width);
    print_truncated(win, line++, 2, free_space_buffer, width);
    print_truncated(win, line++, 2, available_space_buffer, width);
    print_truncated(win, line++, 2, reed_speed_buffer, width);
    print_truncated(win, line++, 2, write_speed_buffer, width);
    print_truncated(win, line,   2, system_type_buffer, width);

    const double MIN_SPACE_GB = 1.0;
    if (space_info->available_space < MIN_SPACE_GB) {
        snprintf(available_space_buffer,sizeof(available_space_buffer),
            "⚠️ Внимание! Мало свободного места (меньше %.2f GB)", MIN_SPACE_GB);
        show_error_dialog(available_space_buffer);
    }
    print_truncated(win, height-2, 2, "Press any key...", width);

    wrefresh(win);
    wgetch(win);
    delwin(win);
}




