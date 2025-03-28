#ifndef APP_H
#define APP_H

#include "../../include/types.h"
#include "../../include/constants.h"
#include "../config/config.h"
#include "../operations/clipboard.h"

#define APP_VERSION "1.0.0"

// Инициализация состояния приложения
void app_init(ApplicationState* state);

// Главный цикл приложения
void app_run(ApplicationState* state);

// Очистка ресурсов
void app_cleanup(ApplicationState* state);

// Обновление текущей директории
void app_refresh_current_tab(ApplicationState* state);

#endif