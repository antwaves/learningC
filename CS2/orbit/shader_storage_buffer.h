#ifndef UNIFORM_BUFFERS
#define UNIFORM_BUFFERS
#include "app.h"

struct shader_storage_object {
    float transformation[2];
};
void update_shader_storage_buffer(void** shader_storage_buffers_mapped, uint32_t current_image, uint32_t vertex_count);
#endif
