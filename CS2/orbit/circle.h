#include <GLFW/glfw3.h>
#include <stdint.h>
#include "app.h"
#include "vertex.h"

#ifndef CIRCLE
#define CIRCLE
struct Circle;
void create_circle_vertices(Vertex* vertex_staging_buffer, uint16_t* index_staging_buffer, GLFWwindow* window, struct Circle c);
void create_circle(struct App* self, struct Circle c);
void create_circle_vertices(Vertex* vertex_staging_buffer, uint16_t* index_staging_buffer, GLFWwindow* window, struct Circle c);

struct Circle {
    uint16_t radius;
    int x;
    int y;
    unsigned char color[3];
};
#endif
