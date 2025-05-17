#include "app.h"
#include "../ui/ui.h"
#include "../ui/dialog.h"
#include "../operations/file_ops.h"
#include "../config/config.h"
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "../input/input.h"

void app_init(ApplicationState* state) {
    memset(state, 0, sizeof(ApplicationState));
    config_load(&state->config);

    for(int i = 0; i < MAX_TABS; i++) {
        if(getcwd(state->tabs[i].path, MAX_PATH_LEN) == NULL) {
            strcpy(state->tabs[i].path, "/");
        }
        load_directory(&state->tabs[i]);
    }

    state->active_tab = 0;
    state->should_exit = 0;
    state->need_refresh = 1;
}

void app_run(ApplicationState* state) {
    while(!state->should_exit) {
        if(state->need_refresh) {
            ui_draw_interface(state);
            state->need_refresh = 0;
        }

        int ch = getch();
        handle_input(state, ch);
    }
    state->need_refresh = 1;
}

void app_refresh_current_tab(ApplicationState* state) {
    Tab* tab = &state->tabs[state->active_tab];
    load_directory(tab);
    state->need_refresh = 1;
}