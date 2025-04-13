#ifndef UI_H
#define UI_H

#include "../../include/types.h"
#include "../config/config.h"

void ui_init();

void ui_cleanup();

// Отрисовка интерфейса
void ui_draw_interface(const ApplicationState* state);

// Обновление размеров
void ui_handle_resize();

void load_directory(Tab* tab);

#endif