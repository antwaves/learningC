#include "app.h"
#include "vulkan/vulkan_core.h"

void _create_command_pool(struct App* self);
void _create_command_buffer(struct App* self);
void transition_image_layout(uint32_t image_index, VkImageLayout old_layout, VkImageLayout new_layout, 
                            VkAccessFlags2 src_access_mask, VkAccessFlags2 dst_access_mask, 
                            VkPipelineStageFlags2 src_stage_mask, VkPipelineStageFlags2 dst_stage_mask);


void _create_command_pool(struct App* self) {
    VkCommandPoolCreateInfo command_pool_create_info = {
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = self->queue_family_index,
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO
    };
    vkCreateCommandPool(self->logical_device, &command_pool_create_info, NULL, &self->command_pool);
}


void _create_command_buffer(struct App* self) {
    VkCommandBufferAllocateInfo alloc_info = {
        .commandPool = self->command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY, 
        .commandBufferCount = 1,
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO
    };

    VkCommandBuffer buffers[1];
    vkAllocateCommandBuffers(self->logical_device, &alloc_info, buffers);
    VkCommandBuffer buffer = buffers[0];
    self->command_buffer = buffer;

    VkCommandBufferBeginInfo vk_command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };
    vkBeginCommandBuffer(self->command_buffer, &vk_command_buffer_begin_info);

    vkEndCommandBuffer(self->command_buffer);
}


void transition_image_layout(uint32_t image_index, VkImageLayout old_layout, VkImageLayout new_layout, 
                            VkAccessFlags2 src_access_mask, VkAccessFlags2 dst_access_mask, 
                            VkPipelineStageFlags2 src_stage_mask, VkPipelineStageFlags2 dst_stage_mask) {
                                
                            }