#include "app.h"
#include "vertex.h"
#include "vulkan/vulkan_core.h"

#include <stdio.h>
#include <stdlib.h>

void _create_command_pools(struct App* self);
void _create_command_buffers(struct App* self);
void _record_command_buffer(struct App* self, uint32_t image_index);
void _transition_image_layout(struct App* self, uint32_t image_index, VkImageLayout old_layout, 
                              VkImageLayout new_layout, VkAccessFlags2 src_access_mask, 
                              VkAccessFlags2 dst_access_mask, VkPipelineStageFlags2 src_stage_mask, 
                              VkPipelineStageFlags2 dst_stage_mask);


void _create_command_pools(struct App* self) { // create our command pool, which manages our command buffer
    VkCommandPoolCreateInfo graphics_command_pool_create_info = {
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = self->graphics_queue_family_index,
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO
    };
    VkCommandPoolCreateInfo transfer_command_pool_create_info = {
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = self->transfer_queue_family_index,
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO
    };

    vkCreateCommandPool(self->logical_device, &graphics_command_pool_create_info, NULL, &self->graphics_command_pool);
    vkCreateCommandPool(self->logical_device, &transfer_command_pool_create_info, NULL, &self->transfer_command_pool);
}


void _create_command_buffers(struct App* self) { // create our array of command buffers, based on our max frames in flight
    VkCommandBufferAllocateInfo alloc_info = {
        .commandPool = self->graphics_command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY, 
        .commandBufferCount = self->MAX_FRAMES_IN_FLIGHT,
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO
    };

    VkCommandBuffer* buffers = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkCommandBuffer));
    if (vkAllocateCommandBuffers(self->logical_device, &alloc_info, buffers) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create command buffers!");
        exit(EXIT_FAILURE);
    }
    self->command_buffers = buffers;
}


void _record_command_buffer(struct App* self, uint32_t image_index) { // set the commands at a specific index in our array of command buffers. 
    VkCommandBufferBeginInfo vk_command_buffer_begin_info = { // commands are put into a buffer, then sent all at once
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };
    vkBeginCommandBuffer(self->command_buffers[self->frame_index], &vk_command_buffer_begin_info); // start reading commands

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
    VkCommandBuffer cmd_buffer = self->command_buffers[self->frame_index];
    vkCmdBeginRendering(cmd_buffer, &rendering_info);
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, self->graphics_pipeline);
    const VkDeviceSize offsets = {0};
    vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &self->vertex_buffer, &offsets);
    vkCmdBindIndexBuffer(cmd_buffer, self->index_buffer, offsets, VK_INDEX_TYPE_UINT16);

    VkViewport viewport[] = {{0.0f, 0.0f, (float)self->swap_chain_extent.width, (float)self->swap_chain_extent.height, 0.0f, 1.0f}};
    vkCmdSetViewport(cmd_buffer, 0, 1, viewport);
    VkRect2D scissor[] = {
        {{0, 0}, self->swap_chain_extent}
    };
    vkCmdSetScissor(cmd_buffer, 0, 1, scissor);
    vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, self->pipeline_layout, 0, 1, self->descriptor_sets, 0, NULL);
    vkCmdDrawIndexed(cmd_buffer, sizeof(indices) / sizeof(uint16_t), 1, 0, 0, 0); //AAAAAaa
    vkCmdEndRendering(cmd_buffer);

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

    vkEndCommandBuffer(self->command_buffers[self->frame_index]); // stop reading commands
}


void _transition_image_layout(struct App* self, uint32_t image_index, VkImageLayout old_layout, // transitions between two image layours
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

    vkCmdPipelineBarrier2(self->command_buffers[self->frame_index], &dependency_info);
}
