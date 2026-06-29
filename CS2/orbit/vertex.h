#include <cglm/cglm.h>
#include "app.h"

#ifndef VERTEX
#define VERTEX

typedef struct {
    vec2 pos;
    vec3 color;
} Vertex;

typedef struct {
    uint32_t count;
    VkVertexInputAttributeDescription* attr_descriptions;
} vertex_in_attr_list;


const Vertex vertices[] = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};


VkVertexInputBindingDescription get_binding_description();
vertex_in_attr_list get_attribute_descriptions();
void _create_vertex_buffer(struct App* self);

uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties, VkPhysicalDevice* physical_device);

#endif
