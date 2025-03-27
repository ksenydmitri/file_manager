#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FILE ".fmrc"

typedef struct {
    struct {
        int dir_color;
        int file_color;
        int selected_color;
    } theme;
    
    struct {
        int show_hidden;
        int human_readable;
    } display;
    
    struct {
        char copy_key[16];
        char delete_key[16];
    } keymap;
} AppConfig;

// Загрузка конфигурации
void config_load(AppConfig* config);

// Сохранение конфигурации
void config_save(const AppConfig* config);

// Установка значений по умолчанию
void config_set_defaults(AppConfig* config);

#endif