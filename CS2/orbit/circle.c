#include <stdint.h>
#include <GLFW/glfw3.h>

#include "vertex.h"

struct Circle {
    uint16_t radius;
    int x;
    int y;
    unsigned char color[3];
};


void create_circle(Vertex* vertex_staging_buffer, uint16_t* index_staging_buffer, GLFWwindow* window, uint16_t x, uint16_t y, uint16_t radius, unsigned char c_color[3]) {
    uint16_t indices[6] = {0};
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    float norm_x = (float)x / width;
    float norm_y = (float)y / height;
    float norm_x_radius = (float)radius / width;
    float norm_y_radius = (float)radius / height;

    vec3 c = {(float)c_color[0] / 255, (float)c_color[1] / 255, (float)c_color[2] / 255};
    Vertex top_left = {{norm_x - norm_x_radius, norm_y - norm_y_radius}, {c[0], c[1], c[2]}, {0.0f, 0.0f}};
    Vertex top_right = {{norm_x + norm_x_radius, norm_y - norm_y_radius}, {c[0], c[1], c[2]}, {1.0f, 0.0f}};
    Vertex bottom_right = {{norm_x + norm_x_radius, norm_y + norm_y_radius}, {c[0], c[1], c[2]}, {1.0f, 1.0f}};
    Vertex bottom_left = {{norm_x - norm_x_radius, norm_y + norm_y_radius}, {c[0], c[1], c[2]}, {0.0f, 1.0f}};

    vertex_staging_buffer[0] = top_left;
    vertex_staging_buffer[1] = top_right;
    vertex_staging_buffer[2] = bottom_right;
    vertex_staging_buffer[3] = bottom_left;

    uint16_t index_temp[6] = {0, 1, 2, 2, 3, 0};
    for (int i = 0; i < 6; i++) {
        index_staging_buffer[i] = index_temp[i];
    }
}

