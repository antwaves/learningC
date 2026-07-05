#include "app.h"
#include "vertex.h"
#include "buffer.h"
#include "shader_storage_buffer.h"

#include <vulkan/vulkan_core.h>
#include <stdint.h>
#include <string.h>
#include <time.h>


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
