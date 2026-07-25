#include "app.c"
#include "circle.c"
#include "planet.c"

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

    struct PlanetArr* p_arr = malloc(sizeof(struct PlanetArr));
    p_arr->planets = calloc(4, sizeof(struct Planet));
    p_arr->num_planets = 4;
    p_arr->planets[0] = p1;
    p_arr->planets[1] = p2;
    p_arr->planets[2] = p3;
    p_arr->planets[3] = p4;

    struct App* app = init(before_initialization, p_arr, before_draw, p_arr);
    app->run(app);
    destroy_app(app);
    free(p_arr->planets);
    free(p_arr);
}


void before_initialization(struct App* app, void* arg) {
    handle_recreation(app, arg);
}


void before_draw(struct App* app, void* arg) {
    static float first_trans = 0.0f;

    struct PlanetArr* p = (struct PlanetArr*)app->before_draw_arg;
    // planet update here

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

    struct PlanetArr arr = *((struct PlanetArr*)arg);
    for (int i = 0; i < arr.num_planets; i++) {
        struct Planet *p = &arr.planets[i];

        update_planet_acceleration(p, arr.planets, arr.num_planets);
        move(p);
   
        for (int j = 0; j < 4; j++) {
            app->user_transformations[i * 4 + j][0] = ((p->circle.x - p->start_pos[0]) / (float)atomic_load(&app->width)) * 2.0f;
            app->user_transformations[i * 4 + j][1] = ((p->circle.y - p->start_pos[1]) / (float)atomic_load(&app->height)) * 2.0f;
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

    struct PlanetArr arr = *((struct PlanetArr*)arg);
    for (int i = 0; i < arr.num_planets; i++) {
        create_circle(app, arr.planets[i].circle);
    }
}