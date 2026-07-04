#include "app.h"
#include "vertex.h"
#include "buffer.h"
#include "vulkan/vulkan_core.h"

#include <stdint.h>
#include <string.h>
#include <time.h>

#define VERTEX_COUNT (sizeof(vertices) / sizeof(Vertex))


struct shader_storage_object {
    float transformation[2];
};


void _create_descriptor_set_layout(struct App* self) {
    VkDescriptorSetLayoutBinding ssbo_layout_binding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };
    VkDescriptorSetLayoutCreateInfo layout_info = {
        .bindingCount = 1,
        .pBindings = &ssbo_layout_binding,
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO
    };
    vkCreateDescriptorSetLayout(self->logical_device, &layout_info, NULL, &self->descriptor_set_layout);
}


void _create_shader_storage_buffers(struct App* self) {
    VkDeviceSize buffer_size = sizeof(struct shader_storage_object) * self->vertex_count;
    self->shader_storage_buffers = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkBuffer));
    self->shader_storage_buffers_memory= calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkDeviceMemory));
    self->shader_storage_buffers_mapped= calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(void*));

    for (size_t i = 0; i <  self->MAX_FRAMES_IN_FLIGHT; i++) {
        int usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        int memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        bool success = create_buffer(&self->shader_storage_buffers[i], &self->shader_storage_buffers_memory[i], self, buffer_size, usage, memory_properties);
        if (!success) {
            break;
        }
        vkMapMemory(self->logical_device, self->shader_storage_buffers_memory[i], 0, buffer_size, 0, &self->shader_storage_buffers_mapped[i]);
    }
}


void update_shader_storage_buffer(void** shader_storage_buffers_mapped, uint32_t current_image, uint32_t vertex_count) {
    struct timespec ts;
    uint64_t current_milliseconds;
    static uint64_t start_milliseconds; 
    static bool first_call = true;

    if (timespec_get(&ts, TIME_UTC) == TIME_UTC){
        if (first_call) {
            first_call = false;
            start_milliseconds = (uint64_t)ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);
        }
        current_milliseconds = (uint64_t)ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);

    }
    uint64_t time_elapsed = (current_milliseconds - start_milliseconds);

    struct shader_storage_object* ssbos = calloc(vertex_count, sizeof(struct shader_storage_object));
    for (int i = 0; i < vertex_count; i++) {
        ssbos[i].transformation[0] = 0.0001 * time_elapsed;
        ssbos[i].transformation[1] = 0.0001 * time_elapsed;
    }
    memcpy(shader_storage_buffers_mapped[current_image], ssbos, sizeof(struct shader_storage_object) * vertex_count);
    free(ssbos);
}


void _create_descriptor_pool(struct App* self) {
    VkDescriptorPoolSize pool_size = { 
        .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
        .descriptorCount = self->MAX_FRAMES_IN_FLIGHT
    };
    VkDescriptorPoolCreateInfo pool_info = {
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = self->MAX_FRAMES_IN_FLIGHT,
        .poolSizeCount = 1,
        .pPoolSizes = &pool_size,
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO
    };
    vkCreateDescriptorPool(self->logical_device, &pool_info, NULL, &self->descriptor_pool);
}


void _create_descriptor_sets(struct App* self) {
    VkDescriptorSetLayout* layouts = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSetLayout));
    for (size_t i = 0; i < self->MAX_FRAMES_IN_FLIGHT; i++) {
       layouts[i] = self->descriptor_set_layout;
    }
    VkDescriptorSetAllocateInfo alloc_info = {
        .descriptorPool = self->descriptor_pool,
        .descriptorSetCount = self->MAX_FRAMES_IN_FLIGHT,
        .pSetLayouts = layouts,
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO
    };
    self->descriptor_sets = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSet));
    vkAllocateDescriptorSets(self->logical_device, &alloc_info, self->descriptor_sets);

    for (size_t i = 0; i < self->MAX_FRAMES_IN_FLIGHT; i++) {
        if (self->vertex_count <= 0 || self->index_count <= 0) {
            break;
        }

        VkDescriptorBufferInfo buffer_info = {
            .buffer = self->shader_storage_buffers[i],
            .offset = 0,
            .range = sizeof(struct shader_storage_object) * self->vertex_count 
        };

        VkWriteDescriptorSet desc_write = {
            .dstSet = self->descriptor_sets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pBufferInfo = &buffer_info,
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET
        };

        vkUpdateDescriptorSets(self->logical_device, 1, &desc_write, 0, NULL);
    }

    free(layouts);
}
