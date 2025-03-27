#ifndef DIALOG_H
#define DIALOG_H

#include "../include/types.h"

// Типы диалогов
typedef enum {
    DIALOG_CONFIRM,
    DIALOG_INPUT,
    DIALOG_ERROR
} DialogType;

// Результат диалога
typedef struct {
    int confirmed;
    char input[256];
} DialogResult;

// Показать диалоговое окно
DialogResult show_dialog(DialogType type, const char* title, const char* message);

#endif