#include "vertex.h"
#include "vulkan/vulkan_core.h"


VkVertexInputBindingDescription get_binding_description() {
    VkVertexInputBindingDescription binding_description = {
        .binding = 0, 
        .stride = sizeof(Vertex), 
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    return binding_description;                                             
}


vertex_in_attr_list get_attribute_descriptions() {
    VkVertexInputAttributeDescription position = {
        .location = 0, 
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT, 
        .offset = offsetof(Vertex, pos)
    };
    VkVertexInputAttributeDescription color = {
        .location = 1, 
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT, 
        .offset = offsetof(Vertex, color)
    };

    VkVertexInputAttributeDescription bindings[] = {position, color};
    vertex_in_attr_list binding_description = {2, bindings};
    return binding_description;
}
