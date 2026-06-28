#include "app.h"
#include "validate.h"
#include "vulkan/vulkan_core.h"


void _create_command_pool(struct App* self);
void _create_command_buffer(struct App* self);
void _record_command_buffer(struct App* self, uint32_t image_index);
void _transition_image_layout(struct App* self, uint32_t image_index, VkImageLayout old_layout, 
                              VkImageLayout new_layout, VkAccessFlags2 src_access_mask, 
                              VkAccessFlags2 dst_access_mask, VkPipelineStageFlags2 src_stage_mask, 
                              VkPipelineStageFlags2 dst_stage_mask);


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
}


void _record_command_buffer(struct App* self, uint32_t image_index) {
    VkCommandBufferBeginInfo vk_command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };
    vkBeginCommandBuffer(self->command_buffer, &vk_command_buffer_begin_info); // start reading commands

    // before starting rendering, transition the swapchain image to vk::ImageLayout::eColorAttachmentOptimal
    _transition_image_layout(
        self,
        image_index, 
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        0, 
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // dstAcessMask
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
    );

    // set up render info
    VkClearColorValue clear_c = {0.0f, 0.0f, 0.0f, 1.0f};
    VkClearValue clear_color = {.color = clear_c};
    VkRenderingAttachmentInfo attachment_info = {
        .imageView = self->swap_chain_image_views[image_index],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = clear_color,
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO
    };
    VkRenderingInfo rendering_info = {
        .renderArea = {
            .offset = {0, 0},
            .extent = self->swap_chain_extent
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachment_info,
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO
    };

    //render commands
    vkCmdBeginRendering(self->command_buffer, &rendering_info);
    vkCmdBindPipeline(self->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, self->graphics_pipeline);
    VkViewport viewport[] = {{0.0f, 0.0f, (float)self->swap_chain_extent.width, (float)self->swap_chain_extent.height, 0.0f, 1.0f}};
    vkCmdSetViewport(self->command_buffer, 0, 1, viewport);
    VkRect2D scissor[] = {
        {{0, 0}, self->swap_chain_extent}
    };
    vkCmdSetScissor(self->command_buffer, 0, 1, scissor);
    vkCmdDraw(self->command_buffer, 3, 1, 0, 0);
    vkCmdEndRendering(self->command_buffer);

    // after  rendering, transition the swapchain image to vk::ImageLayout::ePresentSrcKHR
    _transition_image_layout(
        self,
        image_index, 
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // src acess mask
        0,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
    );

    vkEndCommandBuffer(self->command_buffer); // stop reading commands
}


void _transition_image_layout(struct App* self, uint32_t image_index, VkImageLayout old_layout, 
                              VkImageLayout new_layout, VkAccessFlags2 src_access_mask, 
                              VkAccessFlags2 dst_access_mask, VkPipelineStageFlags2 src_stage_mask, 
                              VkPipelineStageFlags2 dst_stage_mask) {
    VkImageMemoryBarrier2 barrier = {
        .srcStageMask = src_stage_mask,
        .srcAccessMask = src_access_mask,
        .dstStageMask = dst_stage_mask,
        .dstAccessMask = dst_access_mask,
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = self->swap_chain_images[image_index],
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2
    };
    VkDependencyInfo dependency_info = {
        .dependencyFlags = 0,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier,
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO
    };

    vkCmdPipelineBarrier2(self->command_buffer, &dependency_info);
}
