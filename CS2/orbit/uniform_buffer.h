#ifndef UNIFORM_BUFFERS
#define UNIFORM_BUFFERS
#include "app.h"

struct ProjUniformBuffer {
    mat4 proj;
};
void update_uniform_buffers(void** uniform_buffers_mapped, uint32_t current_image, VkExtent2D swap_extent);

#endif
