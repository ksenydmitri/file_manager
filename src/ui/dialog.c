#include "dialog.h"
#include <ncurses.h>
#include <string.h>

DialogResult show_dialog(DialogType type, const char* title, const char* message) {
    DialogResult result = {0};
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Размеры диалога
    int width = 50;
    int height = 7;
    int start_x = (max_x - width) / 2;
    int start_y = (max_y - height) / 2;
    
    WINDOW* dialog_win = newwin(height, width, start_y, start_x);
    keypad(dialog_win, TRUE);
    
    // Рамка
    box(dialog_win, 0, 0);
    wattron(dialog_win, A_BOLD);
    mvwprintw(dialog_win, 0, 2, " %s ", title);
    wattroff(dialog_win, A_BOLD);
    
    // Сообщение
    mvwprintw(dialog_win, 2, 2, "%s", message);
    
    switch(type) {
        case DIALOG_CONFIRM: {
            mvwprintw(dialog_win, height-2, 10, "Yes (Y) / No (N)");
            wrefresh(dialog_win);
            
            int ch;
            while((ch = wgetch(dialog_win)) {
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
            wgetnstr(dialog_win, result.input, 255);
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