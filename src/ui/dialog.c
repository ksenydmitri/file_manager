#include "dialog.h"
#include <ncurses.h>
#include <string.h>

DialogResult show_dialog(DialogType type, const char* title, const char* message) {
    DialogResult result = {0};

    int max_y;
    int max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Dialog dimensions
    int width;
    int height;
    int start_x;
    int start_y;

    width = 50;
    height = 7;
    start_x = (max_x - width) / 2;
    start_y = (max_y - height) / 2;

    WINDOW* dialog_win;
    dialog_win = newwin(height, width, start_y, start_x);
    keypad(dialog_win, TRUE);

    // Draw border
    box(dialog_win, 0, 0);
    wattron(dialog_win, A_BOLD);
    mvwprintw(dialog_win, 0, 2, " %s ", title);
    wattroff(dialog_win, A_BOLD);

    // Show message
    mvwprintw(dialog_win, 2, 2, "%s", message);

    switch(type) {
        case DIALOG_CONFIRM: {
            mvwprintw(dialog_win, height-2, 10, "Yes (Y) / No (N)");
            wrefresh(dialog_win);

            int ch;
            while((ch = wgetch(dialog_win))) {
                if(ch == 'y' || ch == 'Y') {
                    result.confirmed = 1;
                    break;
                }
                if(ch == 'n' || ch == 'N') {
                    result.confirmed = 0;
                    break;
                }
            }
            break;
        }
        case DIALOG_INPUT: {
            echo();
            curs_set(1);
            mvwprintw(dialog_win, height-3, 2, "Input: ");
            wmove(dialog_win, height-3, 9);

            int input_size;
            input_size = 255;
            wgetnstr(dialog_win, result.input, input_size);

            curs_set(0);
            noecho();
            result.confirmed = 1;
            break;
        }
        case DIALOG_ERROR: {
            mvwprintw(dialog_win, height-2, 2, "Press any key...");
            wrefresh(dialog_win);
            wgetch(dialog_win);
            break;
        }
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