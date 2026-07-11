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
    static float first_trans = 0.0f;

    if (app->user_transformation_count != app->vertex_count || app->user_transformations == NULL) {
        float** old = app->user_transformations;
        app->user_transformations = calloc(sizeof(float*), app->vertex_count);
        if (old != NULL) {
            free(old);
            for (int i = 0; i < app->user_transformation_count; i++) {
                free(old[i]);
            }
        }
        app->user_transformation_count = app->vertex_count;
        for (int i = 0; i < app->vertex_count; i++) {
            app->user_transformations[i] = calloc(sizeof(float), 2);
        }
    }

    for (int i = 0; i < app->vertex_count; i++) {
        app->user_transformations[i][0] = first_trans;
        app->user_transformations[i][1] = first_trans;
    }
    first_trans += 0.0001;
    
    if (atomic_load(&app->frame_buffer_resized)) {
        handle_recreation(app);
    }
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