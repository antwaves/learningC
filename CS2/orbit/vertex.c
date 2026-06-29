#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "vertex.h"
#include "app.h"
#include "vulkan/vulkan_core.h"


void _create_vertex_buffer(struct App* self) {
    VkBufferCreateInfo buffer_info = {
        .size = sizeof(vertices),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO
    };
    vkCreateBuffer(self->logical_device, &buffer_info, NULL, &self->vertex_buffer);

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(self->logical_device, self->vertex_buffer, &mem_requirements);
    int mem_property_bit = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkMemoryAllocateInfo alloc_info = {
        .allocationSize = mem_requirements.size, 
        .memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, mem_property_bit, &self->physical_device),
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO
    };
    vkAllocateMemory(self->logical_device, &alloc_info, NULL, &self->vertex_buffer_memory);
    vkBindBufferMemory(self->logical_device, self->vertex_buffer, self->vertex_buffer_memory, 0);

    void* data;
    vkMapMemory(self->logical_device, self->vertex_buffer_memory, 0, buffer_info.size, 0, &data);
    memcpy(data, vertices, buffer_info.size);
}   


uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties, VkPhysicalDevice* physical_device) {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(*physical_device, &mem_properties);

    for (uint32_t i = 0; i <  mem_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    fprintf(stderr, "Failed to find suitable memory type!");
    exit(EXIT_FAILURE);
}


VkVertexInputBindingDescription get_binding_description() {
    VkVertexInputBindingDescription binding_description = {
        .binding = 0, 
        .stride = sizeof(Vertex), 
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    return binding_description;                                             
}


vertex_in_attr_list* get_attribute_descriptions() {
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

    vertex_in_attr_list* binding_description = calloc(1, sizeof(vertex_in_attr_list));
    binding_description->count = 2;
    binding_description->attr_descriptions = calloc(2, sizeof(VkVertexInputAttributeDescription));
    binding_description->attr_descriptions[0] = position;
    binding_description->attr_descriptions[1] = color;

    return binding_description;
}
