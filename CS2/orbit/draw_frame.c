#include "vulkan/vulkan_core.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "app.h"
#include "command_buffer.h"

void _draw_frame(struct App* self);
void _create_sync_objects(struct App* self);


void _draw_frame(struct App* self) {
    if (vkWaitForFences(self->logical_device, 1, &self->draw_fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
        fprintf(stderr, "Failed to wait on fence!");
        exit(EXIT_FAILURE);
    }
    vkResetFences(self->logical_device, 1, &self->draw_fence);
    uint32_t image_index = 0;
    if (vkAcquireNextImageKHR(self->logical_device, self->swap_chain, UINT64_MAX, self->present_complete_semaphore, NULL, &image_index) != VK_SUCCESS) {
        fprintf(stderr, "Failed to grab image!");
        exit(EXIT_FAILURE);
    }

    _record_command_buffer(self, image_index);
    VkPipelineStageFlags wait_destination_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const VkSubmitInfo submit_info = {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &self->present_complete_semaphore,
        .pWaitDstStageMask = &wait_destination_stage_mask,
        .commandBufferCount = 1,
        .pCommandBuffers = &self->command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &self->render_complete_semaphore,
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO
    };

    vkQueueSubmit(self->queue, 1, &submit_info, self->draw_fence);

    const VkPresentInfoKHR present_info_KHR = {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &self->render_complete_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &self->swap_chain,
        .pImageIndices = &image_index,
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR
    };
    vkQueuePresentKHR(self->queue, &present_info_KHR);


}

void _create_sync_objects(struct App* self) {
    VkSemaphoreCreateInfo semaphore_create_info = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VkFenceCreateInfo fence_create_info = {.flags = VK_FENCE_CREATE_SIGNALED_BIT, .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    vkCreateSemaphore(self->logical_device, &semaphore_create_info, NULL, &self->present_complete_semaphore);
    vkCreateSemaphore(self->logical_device, &semaphore_create_info, NULL, &self->render_complete_semaphore);
    vkCreateFence(self->logical_device, &fence_create_info, NULL, &self->draw_fence);
}