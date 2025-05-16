#ifndef ERROR_H
#define ERROR_H

#include "../../include/enums.h"
#include <stdarg.h>
#include "../../include/constants.h"

typedef struct {
    ErrorCode code;
    char message[256];
    char file[MAX_PATH_LEN];
    int line;
} ErrorInfo;

// Инициализация системы ошибок
void error_init();

// Обработка ошибки
void error_handle(ErrorCode code, const char* file, int line, const char* message);

// Отображение последней ошибки
void error_show_last();

// Получение строкового описания кода ошибки
const char* error_code_to_string(ErrorCode code);

void show_error_dialog(const char* message);

#endif