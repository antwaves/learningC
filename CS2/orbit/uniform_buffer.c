#include "app.h"
#include "vertex.h"
#include "buffer.h"
#include <stdint.h>

#include <string.h>
#include <time.h>


struct uniform_buffer_object {
    float transformation[2];
};


void _create_descriptor_set_layout(struct App* self) {
    VkDescriptorSetLayoutBinding ubo_layout_binding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    VkDescriptorSetLayoutCreateInfo layout_info = {
        .bindingCount = 1,
        .pBindings = &ubo_layout_binding
    };
    vkCreateDescriptorSetLayout(self->logical_device, &layout_info, NULL, &self->descriptor_set_layout);
}


void _create_uniform_buffers(struct App* self) {
    VkDeviceSize buffer_size = sizeof(struct uniform_buffer_object);
    self->uniform_buffers = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkBuffer));
    self->uniform_buffers_memory = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkDeviceMemory));
    self->uniform_buffers_mapped = calloc(self->MAX_FRAMES_IN_FLIGHT, buffer_size);

    for (size_t i = 0; i <  self->MAX_FRAMES_IN_FLIGHT; i++) {
        VkBuffer buffer = self->uniform_buffers[i];
        VkDeviceMemory buffer_memory = self->uniform_buffers_memory[i];
        int usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        int memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        create_buffer(&buffer, &buffer_memory, self, buffer_size, usage, memory_properties);

        void* data = (void*)(&(((struct uniform_buffer_object*)self->uniform_buffers_mapped)[i]));
        vkMapMemory(self->logical_device, buffer_memory, 0, buffer_size , 0, &data);
    }
}


void _update_uniform_buffer(struct App* self, uint32_t current_image) {
    struct timespec ts;
    static uint64_t* start_seconds = NULL;
    uint64_t current_seconds;
    if (timespec_get(&ts, TIME_UTC) == TIME_UTC){
        if (start_seconds == NULL) {
            *start_seconds = ts.tv_sec;
        }
        current_seconds = ts.tv_sec;
    }
    uint64_t time_elapsed = (current_seconds - (*start_seconds);

    struct uniform_buffer_object ubo = {0};
    ubo.transformation[0] = 0.05 * time_elapsed;
    ubo.transformation[1] = 0.05 * time_elapsed;

    memcpy(&self->uniform_buffers_mapped[current_image], &ubo, sizeof(ubo));
}