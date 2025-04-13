#include "dialog.h"
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../../include/types.h"
#include "ui.h"
#include "../operations/file_ops.h"


static void handle_input_dialog(WINDOW* win, int height, DialogResult* result)
{
    echo();
    curs_set(1);

    mvwprintw(win, height-3, 2, "Input: ");
    wmove(win, height-3, 9);

    const int input_size = 255;
    wgetnstr(win, result->input, input_size);

    curs_set(0);
    noecho();
    result->confirmed = 1;
}

static void handle_error_dialog(WINDOW* win, int height)
{
    mvwprintw(win, height-2, 2, "Press any key...");
    wrefresh(win);
    wgetch(win);
}

static void handle_confirm_dialog(WINDOW* win, int height, DialogResult* result)
{
    mvwprintw(win, height-2, 10, "Yes (Y) / No (N)");
    wrefresh(win);

    int done = 0;
    while (!done) {
        const int ch = wgetch(win);

        switch (tolower(ch)) {
            case 'y':
                result->confirmed = 1;
                done = 1;
                break;

            case 'n':
                result->confirmed = 0;
                done = 1;
                break;

            default:
                break;
        }
    }
}

DialogResult show_dialog(DialogType type, const char* title, const char* message)
{
    DialogResult result = {0};
    const int width = 50;
    const int height = 7;

    int max_y;
    int max_x;
    getmaxyx(stdscr, max_y, max_x);

    const int start_x = (max_x - width) / 2;
    const int start_y = (max_y - height) / 2;

    // Create dialog window
    WINDOW* dialog_win = newwin(height, width, start_y, start_x);
    if (!dialog_win) {
        return result;
    }

    keypad(dialog_win, TRUE);
    box(dialog_win, 0, 0);

    // Draw title
    wattron(dialog_win, A_BOLD);
    mvwprintw(dialog_win, 0, 2, " %s ", title);
    wattroff(dialog_win, A_BOLD);

    // Show message
    mvwprintw(dialog_win, 2, 2, "%s", message);

    // Handle different dialog types
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
        default:
              break;
    }
    delwin(dialog_win);
    return result;
}

void show_error_dialog(const char* message) {
    WINDOW* win = newwin(5, 60, (LINES-5)/2, (COLS-60)/2);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "Error:");
    mvwprintw(win, 2, 2, "%.56s", message);
    mvwprintw(win, 3, 2, "Press any key to continue...");
    wrefresh(win);
    getch();
    delwin(win);
}

void show_create_object_dialog(ApplicationState* state) {
    DialogResult type_result = show_dialog(DIALOG_CONFIRM, "Create Object", "Create File (Y) or Directory (N)?");

    if (!type_result.confirmed) {
        return;
    }

    char type = tolower(type_result.input[0]);
    if (type != 'f' && type != 'd') {
        show_error_dialog("Invalid choice. Press 'F' for File or 'D' for Directory.");
        return;
    }

    DialogResult name_result = show_dialog(DIALOG_INPUT, "Create Object", "Enter name for the new object:");
    if (!name_result.confirmed || strlen(name_result.input) == 0) {
        return; // Пользователь отменил
    }

    Tab* tab = &state->tabs[state->active_tab];
    char new_path[MAX_PATH_LEN];
    snprintf(new_path, MAX_PATH_LEN, "%s/%s", tab->path, name_result.input);

    if (type == 'f') {
        if (file_create(new_path) != 0) {
            show_error_dialog("Failed to create file.");
        }
    } else if (type == 'd') {
        if (dir_create(new_path) != 0) {
            show_error_dialog("Failed to create directory.");
        }
    }

    load_directory(tab);
}

void display_search_results(const FileSearchResult* results) {
    // Очистка основного окна интерфейса
    werase(stdscr);

    // Заголовок результатов
    mvprintw(0, 0, "Search Results:");
    int line = 2;

    if (results->count == 0) {
        mvprintw(line, 0, "No files found.");
    } else {
        for (int i = 0; i < results->count; i++) {
            mvprintw(line++, 0, "File: %s", results->files[i].name);
            mvprintw(line++, 0, "Path: %s", results->files[i].path);
        }
    }

    refresh();
}

void show_search_dialog(ApplicationState* state) {
    DialogResult result = show_dialog(DIALOG_INPUT, "Search Files", "Enter file name to search:");

    if (!result.confirmed || strlen(result.input) == 0) {
        return;
    }

    FileSearchResult search_results;
    perform_file_search(state->tabs[state->active_tab].path, result.input, &search_results);
    display_search_results(&search_results);
}

void show_file_contents_dialog(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        show_error_dialog("Failed to open file.");
        return;
    }

    // Подготовка окна для отображения содержимого
    const int width = 80;
    const int height = 20;
    const int padding = 2;

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

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

    mvwprintw(dialog_win, 0, 2, " Viewing: %s ", filepath);
    wrefresh(dialog_win);

    // Чтение содержимого файла и вывод
    char buffer[256];
    int line = 1;
    int ch;
    int scroll_offset = 0;

    // Считаем количество строк в файле для прокрутки
    int total_lines = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        total_lines++;
    }
    rewind(file); // Возвращаем указатель на начало файла

    while (1) {
        werase(dialog_win);
        box(dialog_win, 0, 0);
        mvwprintw(dialog_win, 0, 2, " Viewing: %s ", filepath);

        // Отображение видимой части файла
        line = 1;
        int current_line = 0;
        while (fgets(buffer, sizeof(buffer), file)) {
            if (current_line >= scroll_offset && current_line < scroll_offset + height - padding) {
                mvwprintw(dialog_win, line++, 2, "%s", buffer);
            }
            current_line++;
        }
        wrefresh(dialog_win);
        rewind(file);

        // Обработка ввода клавиш для прокрутки
        ch = wgetch(dialog_win);
        if (ch == 'q') { // Нажатие 'q' для выхода
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

void show_file_dialog(ApplicationState* state, const char* path, FileEntry* entry){
     char filename[MAX_PATH_LEN];
     snprintf(filename, MAX_PATH_LEN, "%s/%s", path, entry->name);
     show_file_contents_dialog(filename);
}