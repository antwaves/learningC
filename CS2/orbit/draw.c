#include "app.c"
#include <stdio.h>

int main() {
    printf("sanity check");
    struct App* app = init();
    app->run(app);
    destroy_app(app);

    return 0;
}
