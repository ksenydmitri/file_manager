#ifndef DIALOG_H
#define DIALOG_H

#include "../../include/types.h"
#include "../../include/enums.h"
#include <ncurses.h>

// Показать диалоговое окно
DialogResult show_dialog(DialogType type, const char* title, const char* message);

void show_create_object_dialog(ApplicationState* state);
void show_search_dialog(ApplicationState* state);
void show_file_dialog(ApplicationState* state, const char* path, FileEntry* entry);

#endif