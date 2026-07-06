#include <stdint.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <cglm/cglm.h>

#include "app.h"
#include "vertex.h"


struct Circle;
void create_circle_vertices(Vertex* vertex_staging_buffer, uint16_t* index_staging_buffer, GLFWwindow* window, struct Circle c);


struct Circle {
    uint16_t radius;
    int x;
    int y;
    unsigned char color[3];
};



void create_circle(struct App* self, struct Circle c) {
    Vertex* vertex_staging = calloc(4, sizeof(Vertex));
    uint16_t* index_staging = calloc(6, sizeof(uint16_t));
    create_circle_vertices(vertex_staging, index_staging, self->window, c);

    uint16_t new_start_index = self->vertex_count;
    for (int i = 0; i < 6; i++) {
        index_staging[i] += new_start_index;
    }

    Vertex* old_vertices = self->vertices;
    uint16_t* old_indices = self->indices;

    self->vertex_count += 4;
    self->index_count += 6;
    self->vertices = calloc(self->vertex_count, sizeof(Vertex));
    self->indices = calloc(self->index_count, sizeof(uint16_t));

    if (old_vertices != NULL && old_indices != NULL && self->vertex_count - 4 != 0 && self->index_count - 6 != 0) {
        memcpy((char*)self->vertices, old_vertices, (sizeof(Vertex) * self->vertex_count - 4));
        memcpy((char*)self->indices, old_indices, (sizeof(uint16_t) * self->index_count - 6));
    }

    memcpy(&self->vertices[self->vertex_count - 4], vertex_staging, 4 * sizeof(Vertex));
    memcpy(&self->indices[self->index_count - 6], index_staging, 6 * sizeof(uint16_t));
    
    free(old_vertices);
    free(old_indices);
    free(vertex_staging);
    free(index_staging);
}


void create_circle_vertices(Vertex* vertex_staging_buffer, uint16_t* index_staging_buffer, GLFWwindow* window, struct Circle c) {
    uint16_t indices[6] = {0};
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    float norm_x = (float)c.x / width;
    float norm_y = (float)c.y / height;
    float norm_x_radius = (float)c.radius / width;
    float norm_y_radius = (float)c.radius / height;

    vec3 color = {(float)c.color[0] / 255, (float)c.color[1] / 255, (float)c.color[2] / 255};
    Vertex top_left = {{norm_x - norm_x_radius, norm_y - norm_y_radius}, {color[0], color[1], color[2]}, {0.0f, 0.0f}};
    Vertex top_right = {{norm_x + norm_x_radius, norm_y - norm_y_radius}, {color[0], color[1], color[2]}, {1.0f, 0.0f}};
    Vertex bottom_right = {{norm_x + norm_x_radius, norm_y + norm_y_radius}, {color[0], color[1], color[2]}, {1.0f, 1.0f}};
    Vertex bottom_left = {{norm_x - norm_x_radius, norm_y + norm_y_radius}, {color[0], color[1], color[2]}, {0.0f, 1.0f}};

    vertex_staging_buffer[0] = top_left;
    vertex_staging_buffer[1] = top_right;
    vertex_staging_buffer[2] = bottom_right;
    vertex_staging_buffer[3] = bottom_left;

    uint16_t index_temp[6] = {0, 1, 2, 2, 3, 0};
    for (int i = 0; i < 6; i++) {
        index_staging_buffer[i] = index_temp[i];
    }
}
