#ifndef APP_H
#define APP_H

#include "../../include/types.h"

#define APP_VERSION "1.0.0"

void app_init(ApplicationState* state);
void app_run(ApplicationState* state);
void app_cleanup(ApplicationState* state);
void app_refresh_current_tab(ApplicationState* state);

#endif