#ifndef UI_H
#define UI_H

#include "../../include/types.h"

void ui_init();
void ui_cleanup();
void ui_draw_interface(const ApplicationState* state);
void ui_handle_resize();
void load_directory(Tab* tab);

#endif