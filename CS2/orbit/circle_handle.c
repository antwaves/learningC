#include "circle.c"
#include <stdint.h>

struct circle_handle;
struct circle_handler;

struct circle_handler {
    struct circle_handle* handles;
};


struct circle_handle {
    struct Circle circle;
    uint32_t start_vertex_range;
    uint32_t start_index_range;
    ivec2 transformation;
};
