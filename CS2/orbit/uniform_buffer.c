#include "app.h"
#include "vertex.h"
#include "buffer.h"
#include "vulkan/vulkan_core.h"

#include <stdint.h>
#include <string.h>
#include <time.h>
#include <vcruntime.h>


struct uniform_buffer_object {
    float transformation[2];
};


void _create_descriptor_set_layout(struct App* self, VkDescriptorSetLayout* p_layout) {
    VkDescriptorSetLayoutBinding ubo_layout_binding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };
    VkDescriptorSetLayoutCreateInfo layout_info = {
        .bindingCount = 1,
        .pBindings = &ubo_layout_binding,
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO
    };
    vkCreateDescriptorSetLayout(self->logical_device, &layout_info, NULL, p_layout);
}


void _create_uniform_buffers(struct App* self) {
    VkDeviceSize buffer_size = sizeof(struct uniform_buffer_object);
    self->uniform_buffers = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkBuffer));
    self->uniform_buffers_memory = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkDeviceMemory));
    self->uniform_buffers_mapped = calloc(self->MAX_FRAMES_IN_FLIGHT, buffer_size);

    for (size_t i = 0; i <  self->MAX_FRAMES_IN_FLIGHT; i++) {
        VkBuffer* buffer = &self->uniform_buffers[i];
        VkDeviceMemory* buffer_memory = &self->uniform_buffers_memory[i];
        int usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        int memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        create_buffer(buffer, buffer_memory, self, buffer_size, usage, memory_properties);

        void* data;
        vkMapMemory(self->logical_device, *buffer_memory, 0, buffer_size, 0, &data);
    }
}


void update_uniform_buffer(void* uniform_buffers_mapped, uint32_t current_image) {
    struct timespec ts;
    uint64_t current_seconds;
    static uint64_t start_seconds; 
    static bool first_call = true;

    if (timespec_get(&ts, TIME_UTC) == TIME_UTC){
        if (first_call) {
            first_call = false;
            start_seconds = ts.tv_sec;
        }
        current_seconds = ts.tv_sec;

    }
    uint64_t time_elapsed = (current_seconds - start_seconds);

    printf("%llu", time_elapsed);
    struct uniform_buffer_object ubo = {0};
    ubo.transformation[0] = time_elapsed;
    ubo.transformation[1] = time_elapsed;

    memcpy((char*)uniform_buffers_mapped + current_image * sizeof(struct uniform_buffer_object), &ubo, sizeof(ubo));
}


void _create_descriptor_pool(struct App* self) {
    VkDescriptorPoolSize pool_size = { 
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
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
        _create_descriptor_set_layout(self, &layouts[i]);
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
        VkDescriptorBufferInfo buffer_info = {
            .buffer = self->uniform_buffers[i],
            .offset = 0, 
            .range = sizeof(struct uniform_buffer_object)
        };
        VkWriteDescriptorSet desc_write = {
            .dstSet = self->descriptor_sets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &buffer_info,
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET
        };
        vkUpdateDescriptorSets(self->logical_device, 1, &desc_write, 0, NULL);
    }
}
