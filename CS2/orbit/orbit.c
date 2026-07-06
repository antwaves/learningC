#include "app.c"
#include "circle.c"

#include <time.h>
#include <stdatomic.h>

void before_initialization(struct App* app);
void before_draw(struct App* app);
void handle_recreation(struct App* app);

int main() {
    struct App* app = init(before_initialization, before_draw);
    app->run(app);
    destroy_app(app);
}


void before_initialization(struct App* app) {
    handle_recreation(app);
}


void before_draw(struct App* app) {
    if (atomic_load(&app->frame_buffer_resized)) {
        handle_recreation(app);
    }
}


void after_resize(struct App* app) {
    handle_recreation(app);
}


void handle_recreation(struct App* app) {
    if (app->vertices != NULL && app->indices != NULL) {
        free(app->vertices);
        free(app->indices);
        app->vertices = NULL;
        app->indices = NULL;
    }
    app->index_count = 0;
    app->vertex_count = 0;
    struct Circle d = {.radius = 40, .x = 10, .y = 10, .color = {10, 20, 100}};
    struct Circle e = {.radius = 60, .x = 100, .y = 10, .color = {100, 20, 100}};

    create_circle(app, d);
    create_circle(app, e);
}