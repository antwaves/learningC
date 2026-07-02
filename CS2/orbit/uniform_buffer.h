#ifndef UNIFORM_BUFFERS
#define UNIFORM_BUFFERS
#include "app.h"

void _create_descriptor_set_layout(struct App* self);
void _create_uniform_buffers(struct App* self);
void update_uniform_buffer(void** uniform_buffers_mapped, uint32_t current_image);
void _create_descriptor_pool(struct App* self);
#endif
