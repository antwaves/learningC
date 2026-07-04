#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

#include "vertex.h"
#include "app.h"
#include "buffer.c"


void _create_vertex_buffer(struct App* self) {
    int buffer_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    int mem_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkDeviceSize buffer_size = self->vertex_count * sizeof(Vertex);
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    bool success = create_buffer(&staging_buffer, &staging_buffer_memory, self, buffer_size, buffer_usage, mem_properties);
    if (!success) {
        printf("WARNING: Missing vertices\n");
        return;
    }

    void* data;
    vkMapMemory(self->logical_device, staging_buffer_memory, 0, buffer_size , 0, &data);
    memcpy(data, self->vertices, buffer_size);
    vkUnmapMemory(self->logical_device, staging_buffer_memory);

    buffer_usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    mem_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    create_buffer(&self->vertex_buffer,& self->vertex_buffer_memory, self, buffer_size, buffer_usage, mem_properties);
    copy_buffer(staging_buffer, self->vertex_buffer, self, buffer_size);

    vkFreeMemory(self->logical_device, staging_buffer_memory, NULL);
    vkDestroyBuffer(self->logical_device, staging_buffer, NULL);
}   


void _create_index_buffer(struct App* self) {
    VkDeviceSize buffer_size = self->index_count * sizeof(uint16_t);
    int buffer_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    int mem_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    bool success = create_buffer(&staging_buffer, &staging_buffer_memory, self, buffer_size, buffer_usage, mem_properties);
    if (!success) {
        printf("WARNING: Missing indices\n");
        return;
    }

    void* data;
    vkMapMemory(self->logical_device, staging_buffer_memory, 0, buffer_size , 0, &data);
    memcpy(data, self->indices, buffer_size);
    vkUnmapMemory(self->logical_device, staging_buffer_memory);

    buffer_usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    mem_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    create_buffer(&self->index_buffer,& self->index_buffer_memory, self, buffer_size, buffer_usage, mem_properties);
    copy_buffer(staging_buffer, self->index_buffer, self, buffer_size);
    
    vkFreeMemory(self->logical_device, staging_buffer_memory, NULL);
    vkDestroyBuffer(self->logical_device, staging_buffer, NULL);
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
    VkVertexInputAttributeDescription uv = {
        .location = 2,
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex, uv)
    };

    vertex_in_attr_list* binding_description = calloc(1, sizeof(vertex_in_attr_list));
    binding_description->count = 3;
    binding_description->attr_descriptions = calloc(3, sizeof(VkVertexInputAttributeDescription));
    binding_description->attr_descriptions[0] = position;
    binding_description->attr_descriptions[1] = color;
    binding_description->attr_descriptions[2] = uv;

    return binding_description;
}
