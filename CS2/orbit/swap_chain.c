#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vulkan/vulkan_core.h>

#include "app.h"
#include "swap_chain.h"
#include "timing.h"

#define clamp(d, min, max) (d < min ? min : d) > max ? max : (d < min ? min : d)


void _create_swap_chain(struct App* self) {
    // query the surface capabilities
    VkSurfaceCapabilitiesKHR* surface_capabilities = calloc(1, sizeof(VkSurfaceCapabilitiesKHR));
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(self->physical_device, self->surface, surface_capabilities);
    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(self->physical_device, self->surface, &format_count, NULL);
    assert(format_count != 0);
    VkSurfaceFormatKHR* available_formats = calloc(format_count, sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(self->physical_device, self->surface, &format_count, available_formats);
    uint32_t mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(self->physical_device, self->surface, &mode_count, NULL);
    VkPresentModeKHR* available_present_modes = calloc(mode_count, sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(self->physical_device, self->surface, &mode_count, available_present_modes);
    // pick surface settings
    VkSurfaceFormatKHR swap_format = choose_swap_surface_format(available_formats, format_count);
    VkPresentModeKHR present_mode = choose_present_mode(available_present_modes, mode_count);
    VkExtent2D swap_extent = choose_swap_extent(self->window, surface_capabilities);
    uint32_t min_image_count = choose_swap_min_image_count(surface_capabilities);
    // change sharing mode based on whether or not a transfer queue is available
    uint32_t queue_family_indices[] = {self->graphics_queue_family_index, self->transfer_queue_family_index};
    uint32_t queue_family_index_count = 1 + (self->graphics_queue_family_index != self->transfer_queue_family_index);
    int sharing_mode = self->graphics_queue_family_index == self->transfer_queue_family_index ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
    //create the actual swap chain
    VkSwapchainCreateInfoKHR swap_chain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = self->surface,
        .minImageCount = min_image_count,
        .imageFormat = swap_format.format,
        .imageColorSpace = swap_format.colorSpace,
        .imageExtent = swap_extent, 
        .imageArrayLayers = 1,
        .imageUsage =  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
        .imageSharingMode = sharing_mode,
        .queueFamilyIndexCount = queue_family_index_count,
        .pQueueFamilyIndices = queue_family_indices,
        .preTransform = surface_capabilities->currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = true,
        .oldSwapchain = self->swap_chain // the swap chain that we are replacing, in the case of resizing
    };    
    VkResult result = vkCreateSwapchainKHR(self->logical_device, &swap_chain_info, NULL, &self->swap_chain);
    if (result != VK_SUCCESS) {
        fprintf_s(stderr, "Failed to create swap_chain!");
        exit(EXIT_FAILURE);
    }
    // get the queue of images that we can write to from the swap chain
    vkGetSwapchainImagesKHR(self->logical_device, self->swap_chain, &self->swap_chain_image_count, NULL);
    self->swap_chain_images = calloc(self->swap_chain_image_count, sizeof(VkImage));
    if (vkGetSwapchainImagesKHR(self->logical_device, self->swap_chain, &self->swap_chain_image_count, self->swap_chain_images) != VK_SUCCESS) {
        fprintf_s(stderr, "Failed to grab swap chain images!");
        exit(EXIT_FAILURE);
    }
    self->swap_chain_surface_format = swap_format;
    self->swap_chain_extent = swap_extent;

    free(available_formats);
    free(surface_capabilities);
    free(available_present_modes);
}


static VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR* available_formats, uint32_t items) { // pick format, we want SRGB if possible
    VkSurfaceFormatKHR* format_preferred = NULL;
    for (int i = 0; i < items; i++) {
        VkSurfaceFormatKHR f = available_formats[i];
        if (f.format == VK_FORMAT_R8G8B8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)  {
            format_preferred = &f;
            break;
        }
    }
    return format_preferred == NULL ? available_formats[0] : *format_preferred;
}


static VkPresentModeKHR choose_present_mode(VkPresentModeKHR* available_modes, uint32_t items) { // pick presentation mode, we want mailbox if possible
    VkPresentModeKHR* mode_preferred = NULL;
    for (int i = 0; i < items; i++) {
        if (available_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) { // mailbox means queued images can be replaced with newer ones
            mode_preferred = &available_modes[i];
        }
    }
    return mode_preferred == NULL ? VK_PRESENT_MODE_FIFO_KHR : *mode_preferred;
}


static VkExtent2D choose_swap_extent(GLFWwindow* window, VkSurfaceCapabilitiesKHR *capabilities) { // choose how large the swap chain surface is
    VkSurfaceCapabilitiesKHR c = *capabilities;
    if (c.currentExtent.width !=  UINT32_MAX) { // some window managers use UINT32_MAX to allow us to differ from our capabilities' extent
        return c.currentExtent;
    }
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    VkExtent2D extent = {clamp(width, c.minImageExtent.width, c.maxImageExtent.width), 
                         clamp(height, c.minImageExtent.height, c.maxImageExtent.width)};
    return extent;
}


static uint32_t choose_swap_min_image_count(VkSurfaceCapabilitiesKHR* capabilities) { // choose the amount of images in the queue
    uint32_t min_image_count = max(3u, capabilities->minImageCount);
    if ((0 < capabilities->maxImageCount) && capabilities->maxImageCount < min_image_count) {
        min_image_count = capabilities->maxImageCount;
    }
    return min_image_count;
}


void _create_image_views(struct App* self) { // create the images that we write to
    VkImageViewCreateInfo image_view_create_info = {
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = self->swap_chain_surface_format.format,
        .subresourceRange = { //  describes the purpose of the image
            VK_IMAGE_ASPECT_COLOR_BIT, 
            .levelCount = 1,
            .layerCount = 1
        },
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,

    };
    self->swap_chain_image_views = calloc(self->swap_chain_image_count, sizeof(VkImageView));
    for (int i = 0; i < self->swap_chain_image_count; i++) {
        image_view_create_info.image = self->swap_chain_images[i];
        vkCreateImageView(self->logical_device, &image_view_create_info, NULL, &self->swap_chain_image_views[i]);
    }
}


void _cleanup_swap_chain(struct App *self) { // clean up the swap chain, including its images
    vkDeviceWaitIdle(self->logical_device); // need to wait on operations to finish
    for (int i = 0; i < self->swap_chain_image_count; i++) { vkDestroyImageView(self->logical_device, self->swap_chain_image_views[i], NULL); }
    free(self->swap_chain_images);
    vkDestroySwapchainKHR(self->logical_device, self->swap_chain, NULL);
    self->swap_chain = NULL;
}


void _recreate_swap_chain(struct App* self) { // recreate the swap chain upon a resize
    struct timespec ts = {};
    while (self->width == 0 || self->height == 0) {
        precise_sleep(1.0 / 60.0, &ts);
    }
    precise_sleep(1.0 / 60.0, &ts);

    if (self->width == -1 || self->height == -1) {
        printf("escape!");
        return;
    }

    vkDeviceWaitIdle(self->logical_device);
    _cleanup_swap_chain(self);
    _create_swap_chain(self);
    _create_image_views(self);
}
