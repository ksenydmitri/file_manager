#include "config.h"
#include "../utils/string_utils.h"
#include "../../include/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

void config_set_defaults(AppConfig* config) {
    // Initialize default color pairs
    init_pair(1, COLOR_BLUE, COLOR_BLACK);    // Directory color
    init_pair(2, COLOR_WHITE, COLOR_BLACK);   // File color

    config->theme.dir_color = COLOR_PAIR(1);   // Use color pair 1
    config->theme.file_color = COLOR_PAIR(2);  // Use color pair 2
    config->theme.selected_color = A_REVERSE;  // Correct constant for reverse video
    
    config->display.show_hidden = 0;
    config->display.human_readable = 1;
    
    str_copy(config->keymap.copy_key, "F5", sizeof(config->keymap.copy_key));
    str_copy(config->keymap.delete_key, "F8", sizeof(config->keymap.delete_key));
}

void config_load(AppConfig* config) {
    FILE* fp = fopen(CONFIG_FILE, "r");
    if (!fp) {
        config_set_defaults(config);
        return;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char key[64];
        char value[64];

        if (sscanf(line, "%63[^=]=%63[^\n]", key, value) == 2) {
            if (strcmp(key, "theme.dir_color") == 0) {
                config->theme.dir_color = atoi(value);
            }
            else if (strcmp(key, "display.show_hidden") == 0) {
                config->display.show_hidden = atoi(value);
            }
        }
    }
    fclose(fp);
}

void config_save(const AppConfig* config) {
    FILE* fp = fopen(CONFIG_FILE, "w");
    if (!fp) return;
    
    fprintf(fp, "theme.dir_color=%d\n", config->theme.dir_color);
    fprintf(fp, "theme.file_color=%d\n", config->theme.file_color);
    fprintf(fp, "display.show_hidden=%d\n", config->display.show_hidden);
    fprintf(fp, "display.human_readable=%d\n", config->display.human_readable);
    fclose(fp);
}