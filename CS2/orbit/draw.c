
#include <stdint.h>
#include <stdlib.h>

#include "app.c"
void run(struct App* self);


void run(struct App* self) {
    _init_window(self);
    _init_vulkan(self);
    _main_loop(self);
    _clean_up(self);
}


void _create_graphics_pipeline(struct App* self) {


}

struct App* init() {
    struct App* a = calloc(1, sizeof(struct App));
    a->run = run;
    a->WIDTH = 800;
    a->HEIGHT = 600;
    a->log = true;
    return a;
}


void destroy_app(struct App* a) {
    free(a);
}


int main() {
    struct App* app = init();
    app->run(app);
    destroy_app(app);

    return EXIT_SUCCESS;
}
