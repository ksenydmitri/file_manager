#ifndef DIALOG_H
#define DIALOG_H

#include "../../include/types.h"
#include "../../include/enums.h"
#include <ncurses.h>

// Показать диалоговое окно
DialogResult show_dialog(DialogType type, const char* title, const char* message);

#endif