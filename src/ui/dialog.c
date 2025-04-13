#include "dialog.h"
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../../include/types.h"

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
                // Ignore other keys - continue waiting for Y/N
                break;
        }
    }
}

DialogResult show_dialog(DialogType type, const char* title, const char* message)
{
    DialogResult result = {0};

    // Calculate dialog position and size
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