#include "vulkan/vulkan_core.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "validate.h"
#include "app.h"
#include "command_buffer.h"

void _draw_frame(struct App* self);
void _create_sync_objects(struct App* self);

const int MAX_FRAMES_IN_FLIGHT = 2;


void _draw_frame(struct App* self) {
    if (vkWaitForFences(self->logical_device, 1, &self->in_flight_fences[self->frame_index], VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
        fprintf(stderr, "Failed to wait on fence!");
        exit(EXIT_FAILURE);
    }
    vkResetFences(self->logical_device, 1, &self->in_flight_fences[self->frame_index]);
    uint32_t image_index = 0;
    if (vkAcquireNextImageKHR(self->logical_device, self->swap_chain, UINT64_MAX, self->present_complete_semaphores[self->frame_index], NULL, &image_index) != VK_SUCCESS) {
        fprintf(stderr, "Failed to grab image!");
        exit(EXIT_FAILURE);
    }

    _record_command_buffer(self, image_index);
    VkPipelineStageFlags wait_destination_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const VkSubmitInfo submit_info = {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &self->present_complete_semaphores[self->frame_index],
        .pWaitDstStageMask = &wait_destination_stage_mask,
        .commandBufferCount = 1,
        .pCommandBuffers = &self->command_buffers[self->frame_index],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &self->render_complete_semaphores[image_index],
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO
    };

    vkQueueSubmit(self->queue, 1, &submit_info, self->in_flight_fences[self->frame_index]);

    const VkPresentInfoKHR present_info_KHR = {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &self->render_complete_semaphores[image_index],
        .swapchainCount = 1,
        .pSwapchains = &self->swap_chain,
        .pImageIndices = &image_index,
        .pResults = NULL,
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR
    };
    vkQueuePresentKHR(self->queue, &present_info_KHR);

    self->frame_index = (self->frame_index + 1) % self->MAX_FRAMES_IN_FLIGHT;

}

void _create_sync_objects(struct App* self) {
    VkSemaphoreCreateInfo semaphore_create_info = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VkFenceCreateInfo fence_create_info = {.flags = VK_FENCE_CREATE_SIGNALED_BIT, .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};

    self->render_complete_semaphores = calloc(self->swap_chain_image_count, sizeof(VkSemaphore));
    for (size_t i = 0; i < self->swap_chain_image_count; i++) {
        vkCreateSemaphore(self->logical_device, &semaphore_create_info, NULL, &self->render_complete_semaphores[i]);  
    }

    self->present_complete_semaphores = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
    self->in_flight_fences= calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkFence));
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkCreateSemaphore(self->logical_device, &semaphore_create_info, NULL, &self->present_complete_semaphores[i]);
        vkCreateFence(self->logical_device, &fence_create_info, NULL, &self->in_flight_fences[i]);
    }
}
