#ifndef INPUT_H
#define INPUT_H

#include "../../include/types.h"

// Обработка пользовательского ввода
void handle_input(ApplicationState* state, int key);

// Навигация
void navigate_up(ApplicationState* state);
void navigate_down(ApplicationState* state);
void enter_directory(ApplicationState* state);
void go_back(ApplicationState* state);

// Операции с файлами
void delete_selected(ApplicationState* state);
void handle_copy_operation(ApplicationState* state);
void handle_paste_operation(ApplicationState* state);

#endif