#ifndef CMD_BUFFER
#define CMD_BUFFER
#include "app.h"

void _create_command_pool(struct App* self);
void _create_command_buffer(struct App* self);
void _record_command_buffer(struct App* self, uint32_t image_index);
void _transition_image_layout(struct App* self, uint32_t image_index, VkImageLayout old_layout, 
                              VkImageLayout new_layout, VkAccessFlags2 src_access_mask, 
                              VkAccessFlags2 dst_access_mask, VkPipelineStageFlags2 src_stage_mask, 
                              VkPipelineStageFlags2 dst_stage_mask);
#endif