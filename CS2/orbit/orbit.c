#include "app.c"
#include "circle.c"

void before_initialization(struct App* self);
void before_draw(struct App* self);


int main() {
    struct App* app = init(before_initialization, before_draw);
    app->run(app);
    destroy_app(app);
}


void before_initialization(struct App* self) {
    struct Circle c = {.radius = 400, .x = 100, .y = 100, .color  ={20, 20, 20}};
    create_circle(self, c);
    struct Circle d = {.radius = 40, .x = 10, .y = 10, .color = {10, 20, 100}};
    create_circle(self, d);
}


void before_draw(struct App* self) {
    if (self->frame_buffer_resized) {
        free(self->vertices);
        free(self->indices);
        struct Circle c = {.radius = 400, .x = 100, .y = 100, .color  ={20, 20, 20}};
        create_circle(self, c);
    }
}

