#ifndef ERROR_H
#define ERROR_H

#include "enums.h"
#include <stdarg.h>

typedef struct {
    ErrorCode code;
    char message[256];
    const char* file;
    int line;
} ErrorInfo;

// Инициализация системы ошибок
void error_init();

// Обработка ошибки
void error_handle(ErrorCode code, const char* file, int line, const char* format, ...);

// Отображение последней ошибки
void error_show_last();

// Получение строкового описания кода ошибки
const char* error_code_to_string(ErrorCode code);

#endif