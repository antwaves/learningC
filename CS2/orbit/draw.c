#include "app.c"

int main() {
    struct App* app = init();
    app->run(app);
    destroy_app(app);

    return EXIT_SUCCESS;
}
