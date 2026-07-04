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
    Vertex* vertex_staging = calloc(4, sizeof(Vertex));
    uint16_t* index_staging = calloc(6, sizeof(uint16_t));

    unsigned char color[3] = {20, 20, 20};
    create_circle(vertex_staging, index_staging, self->window, 400, 400, 100, color);

    free(vertex_staging);
    free(index_staging);
}


void before_draw(struct App* self) {

    if (self->frame_buffer_resized) {
        Vertex* vertex_staging = calloc(4, sizeof(Vertex));
        uint16_t* index_staging = calloc(6, sizeof(uint16_t));


        unsigned char color[3] = {20, 20, 20};
        create_circle(vertex_staging, index_staging, self->window, 400, 400, 100, color);
        printf("%d %f", index_staging[2], vertex_staging[0].color[0]);

        free(vertex_staging);
        free(index_staging);
    }

}

