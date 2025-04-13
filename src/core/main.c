#include "app.h"
#include "../ui/ui.h"
#include <ncurses.h>
#include <locale.h>


int main() {
    setlocale(LC_ALL, "");

    ApplicationState app;

    ui_init();

    app_init(&app);
    app_run(&app);
    app_cleanup(&app);
    ui_cleanup();

    return 0;
}