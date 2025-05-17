#include "app.h"
#include "../ui/ui.h"
#include <locale.h>


int main() {
    setlocale(LC_ALL, "rus");
    ApplicationState app;
    ui_init();
    app_init(&app);
    app_run(&app);
    ui_cleanup();

    return 0;
}