#include "app.h"
#include "../ui/ui.h"
#include <ncurses.h>

int main() {
    ApplicationState app;

    // Инициализация ncurses
    ui_init();

    // Инициализация состояния приложения
    app_init(&app);

    // Главный цикл
    app_run(&app);

    // Очистка
    app_cleanup(&app);
    ui_cleanup();

    return 0;
}