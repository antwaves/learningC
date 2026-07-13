#include "app.c"
#include "planet.h"

#include <time.h>
#include <stdatomic.h>

void before_initialization(struct App* app, void* arg);
void before_draw(struct App* app, void* arg);
void handle_recreation(struct App* app, void* arg);


int main() {
    
    struct Planet p1 = planet_init(300, 300, 0.012f, -0.0432f, 1000);
    struct Planet p2 = planet_init(300, 400, -2.4f, 0.0f, 5);
    struct Planet p3 = planet_init(550, 300, 0.0f, 2.0f, 20);
    struct Planet p4 = planet_init(565, 300, 0.0f, 3.2f, 1);


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