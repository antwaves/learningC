#include "app.h"
#include "buffer.h"
#include "uniform_buffer.h"

#include <vulkan/vulkan_core.h>
#include <cglm/cglm.h>
#include <string.h>


void _create_uniform_buffers(struct App* self) {
    VkDeviceSize buffer_size = sizeof(struct ProjUniformBuffer);
    self->uniform_buffers = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkBuffer));
    self->uniform_buffers_memory = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkDeviceMemory));
    self->uniform_buffers_mapped = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(void*));

    for (size_t i = 0; i <  self->MAX_FRAMES_IN_FLIGHT; i++) {
        int usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        int memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        bool success = create_buffer(&self->uniform_buffers[i], &self->uniform_buffers_memory[i], self, buffer_size, usage, memory_properties);
        if (!success) {
            break;
        }
        vkMapMemory(self->logical_device, self->uniform_buffers_memory[i], 0, buffer_size, 0, &self->uniform_buffers_mapped[i]);
    }
}


void update_uniform_buffers(void** uniform_buffers_mapped, uint32_t current_image, VkExtent2D swap_extent) {
    struct ProjUniformBuffer ubo = {0};
    float aspect = (float)swap_extent.width / (float)swap_extent.height;  
    glm_ortho(-aspect, aspect, 1.0f, -1.0f, -1.0f, 1.0f, ubo.proj);
    ubo.proj[1][1] *= -1;
    memcpy(uniform_buffers_mapped[current_image], &ubo, sizeof(ubo));
}
