#include <cglm/cglm.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

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


const Vertex verticies[] = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};


VkVertexInputBindingDescription get_binding_description();
vertex_in_attr_list get_attribute_descriptions();

#endif
