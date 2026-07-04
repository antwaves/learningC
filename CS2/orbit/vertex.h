#include <cglm/cglm.h>
#include <vulkan/vulkan_core.h>

#ifndef VERTEX
#define VERTEX

typedef struct {
    vec2 pos;
    vec3 color;
    vec2 uv;
} Vertex;

typedef struct {
    uint32_t count;
    VkVertexInputAttributeDescription* attr_descriptions;
} vertex_in_attr_list;


VkVertexInputBindingDescription get_binding_description();
vertex_in_attr_list* get_attribute_descriptions();

#endif
