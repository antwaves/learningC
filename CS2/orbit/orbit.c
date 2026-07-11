#include "app.c"
#include "circle.c"

#include <time.h>
#include <stdatomic.h>

void before_initialization(struct App* app, void* arg);
void before_draw(struct App* app, void* arg);
void handle_recreation(struct App* app, void* arg);


int main() {
    struct Circle p1 = {.radius = 40, .x = 10, .y = 10, .color = {10, 20, 100}};
    struct Circle p2 = {.radius = 60, .x = 100, .y = 10, .color = {100, 20, 100}};
    struct TransformableCircle circle_one = {.circle = p1, .transformation = {0, 0}};
    struct TransformableCircle circle_two = {.circle = p2, .transformation = {0, 0}};
    struct TransformableCircleArr* circle_arr = malloc(sizeof(struct TransformableCircleArr));
    circle_arr->circles = calloc(2, sizeof(struct TransformableCircle));
    circle_arr->circles[0] = circle_two;
    circle_arr->circles[1] = circle_one;
    circle_arr->count = 2;

    struct App* app = init(before_initialization, circle_arr, before_draw, circle_arr);
    app->run(app);
    destroy_app(app);
    free(circle_arr->circles);
    free(circle_arr);
}


void before_initialization(struct App* app, void* arg) {
    handle_recreation(app, arg);
}


void before_draw(struct App* app, void* arg) {
    static float first_trans = 0.0f;

    struct TransformableCircleArr* c = (struct TransformableCircleArr*)app->before_draw_arg;
    for (int i = 0; i < c->count; i++) {
        c->circles[i].transformation[0] += 1.0f / app->width;
        c->circles[i].transformation[1] += 1.0f / app->height;
    }

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

    struct TransformableCircleArr list = *((struct TransformableCircleArr*)arg);
    for (int i = 0; i < list.count; i++) {
        for (int j = 0; j < 4; j++) {
            app->user_transformations[i * 4 + j][0] = list.circles->transformation[0];
            app->user_transformations[i * 4 + j][1] = list.circles->transformation[1];
        }
    }

    if (atomic_load(&app->frame_buffer_resized)) {
        handle_recreation(app, arg);
    }
}


void handle_recreation(struct App* app, void* arg) { 
    if (app->vertices != NULL && app->indices != NULL) {
        free(app->vertices);
        free(app->indices);
        app->vertices = NULL;
        app->indices = NULL;
    }
    app->index_count = 0;
    app->vertex_count = 0;

    struct TransformableCircleArr list = *((struct TransformableCircleArr*)arg);
    for (int i = 0; i < list.count; i++) {
        create_circle(app, list.circles[i].circle);
    }
}