#include "GLFW/glfw3.h"
#include "vulkan/vulkan_core.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "app.h"
#include "command_buffer.h"
#include "swap_chain.h"

void _draw_frame(struct App* self);
void _create_sync_objects(struct App* self);
static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

const int MAX_FRAMES_IN_FLIGHT = 2;


void _draw_frame(struct App* self) { // draw a frame
    // wait for previous frame to finish using a fence
    VkResult result = vkWaitForFences(self->logical_device, 1, &self->in_flight_fences[self->frame_index], VK_TRUE, UINT64_MAX); // make the CPU wait until the previous frame is finished presenting
    if (result != VK_SUCCESS) { 
        fprintf(stderr, "Failed to wait on fence!");
        exit(EXIT_FAILURE);
    }
    // accquire next swapchain image
    uint32_t image_index = 0;
    result = vkAcquireNextImageKHR(self->logical_device, self->swap_chain, UINT64_MAX, self->present_complete_semaphores[self->frame_index], NULL, &image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) { // if we've changed window sizes, recreate the swap chain to account for it
        _recreate_swap_chain(self);
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        fprintf(stderr, "Failed to grab image!");
        exit(EXIT_FAILURE);
    }
    vkResetFences(self->logical_device, 1, &self->in_flight_fences[self->frame_index]);
    VkCommandBufferResetFlags reset_flags = {0};
    vkResetCommandBuffer(self->command_buffers[self->frame_index], reset_flags);
    // record the command buffer, with the commands drawiing the scene onto the swapchain image
    _record_command_buffer(self, image_index);
    VkPipelineStageFlags wait_destination_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const VkSubmitInfo submit_info = {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &self->present_complete_semaphores[self->frame_index],  // wait on the last usage of this image to stop being presented
        .pWaitDstStageMask = &wait_destination_stage_mask,
        .commandBufferCount = 1,
        .pCommandBuffers = &self->command_buffers[self->frame_index], // get the command buffer for this specific frame
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &self->render_complete_semaphores[image_index], // stop others from acessing this image until we're done rendering
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO
    };
    // submit our commands to the swapchain queue
    vkQueueSubmit(self->queue, 1, &submit_info, self->in_flight_fences[self->frame_index]); // blocks the CPU from rendering another frame and submitting those commands while we're submitting commands
    // describe how to present our image
    const VkPresentInfoKHR present_info_KHR = {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &self->render_complete_semaphores[image_index], // wait on the image to complete rendering
        .swapchainCount = 1,
        .pSwapchains = &self->swap_chain,
        .pImageIndices = &image_index,
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR
    };
    // present our image to the queue, which presents it to the screen
    result = vkQueuePresentKHR(self->queue, &present_info_KHR);
    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR || self->frame_buffer_resized) { // if we've changed window sizes, recreate the swap chain to account for it
        self->frame_buffer_resized = false;
        _recreate_swap_chain(self);
    }
    self->frame_index = (self->frame_index + 1) % self->MAX_FRAMES_IN_FLIGHT; // increment our frame index based on our max 
}


void _create_sync_objects(struct App* self) {
    VkSemaphoreCreateInfo semaphore_create_info = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VkFenceCreateInfo fence_create_info = {.flags = VK_FENCE_CREATE_SIGNALED_BIT, .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};

    self->render_complete_semaphores = calloc(self->swap_chain_image_count, sizeof(VkSemaphore)); // makes the GPU wait to present an image until it's finished rendering
    for (size_t i = 0; i < self->swap_chain_image_count; i++) {
        vkCreateSemaphore(self->logical_device, &semaphore_create_info, NULL, &self->render_complete_semaphores[i]);  
    }

    self->present_complete_semaphores = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore)); // makes the GPU wait for the presentation engine to stop using a image
    self->in_flight_fences= calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkFence)); // makes the CPU wait to process the next image until the last one has finished presenting
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkCreateSemaphore(self->logical_device, &semaphore_create_info, NULL, &self->present_complete_semaphores[i]);
        vkCreateFence(self->logical_device, &fence_create_info, NULL, &self->in_flight_fences[i]);
    }
}


void framebuffer_resize_callback(GLFWwindow* window, int width, int height) { // called when the window is resized, informs other functions
    struct App* self = (struct App*)(glfwGetWindowUserPointer(window));
    self->frame_buffer_resized = true;
    self->width = width;
    self->height = height;
}
