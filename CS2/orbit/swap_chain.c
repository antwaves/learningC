#include <stdio.h>
#include <stdlib.h>
#include "app.h"
#include "vulkan/vulkan_core.h"

#include "swap_chain.h"

#define clamp(d, min, max) (d < min ? min : d) > max ? max : (d < min ? min : d)


void _create_swap_chain(struct App* self) {
    VkSurfaceCapabilitiesKHR* surface_capabilities = calloc(1, sizeof(VkSurfaceCapabilitiesKHR));
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(self->physical_device, self->surface, surface_capabilities);
    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(self->physical_device, self->surface, &format_count, NULL);
    VkSurfaceFormatKHR* available_formats = calloc(format_count, sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(self->physical_device, self->surface, &format_count, available_formats);
    uint32_t mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(self->physical_device, self->surface, &mode_count, NULL);
    VkPresentModeKHR* available_present_modes = calloc(mode_count, sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(self->physical_device, self->surface, &mode_count, available_present_modes);

    VkSurfaceFormatKHR swap_format = choose_swap_surface_format(available_formats, format_count);
    VkPresentModeKHR present_mode = choose_present_mode(available_present_modes, mode_count);
    VkExtent2D swap_extent = choose_swap_extent(self->window, surface_capabilities);
    uint32_t min_image_count = choose_swap_min_image_count(surface_capabilities);

    VkSwapchainCreateInfoKHR swap_chain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = self->surface,
        .minImageCount = min_image_count,
        .imageFormat = swap_format.format,
        .imageColorSpace = swap_format.colorSpace,
        .imageExtent = swap_extent, 
        .imageArrayLayers = 1,
        .imageUsage =  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = surface_capabilities->currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = true,
        .oldSwapchain = self->swap_chain
    };    
    
    if (vkCreateSwapchainKHR(self->logical_device, &swap_chain_info, NULL, &self->swap_chain) != VK_SUCCESS) {
        fprintf_s(stderr, "Failed to create swap_chain!");
        exit(EXIT_FAILURE);
    }
    
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


static VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR* available_formats, uint32_t items) {
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


static VkPresentModeKHR choose_present_mode(VkPresentModeKHR* available_modes, uint32_t items) {
    VkPresentModeKHR* mode_preferred = NULL;
    for (int i = 0; i < items; i++) {
        if (available_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            mode_preferred = &available_modes[i];
        }
    }
    return mode_preferred == NULL ? VK_PRESENT_MODE_FIFO_KHR : *mode_preferred;
}


static VkExtent2D choose_swap_extent(GLFWwindow* window, VkSurfaceCapabilitiesKHR *capabilities) {
    VkSurfaceCapabilitiesKHR c = *capabilities;
    if (c.currentExtent.width !=  UINT32_MAX) {
        return c.currentExtent;
    }
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    VkExtent2D extent = {clamp(width, c.minImageExtent.width, c.maxImageExtent.width), 
                         clamp(height, c.minImageExtent.height, c.maxImageExtent.width)};
    return extent;
}


static uint32_t choose_swap_min_image_count(VkSurfaceCapabilitiesKHR* capabilities) {
    uint32_t min_image_count = max(3u, capabilities->minImageCount);
    if ((0 < capabilities->maxImageCount) && capabilities->maxImageCount < min_image_count) {
        min_image_count = capabilities->maxImageCount;
    }
    return min_image_count;
}


void _create_image_views(struct App* self) {
    VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = self->swap_chain_surface_format.format,
        .subresourceRange = {
            VK_IMAGE_ASPECT_COLOR_BIT, 
            .levelCount = 1,
            .layerCount = 1}
    };

    self->swap_chain_image_views = calloc(self->swap_chain_image_count, sizeof(VkImageView));
    for (int i = 0; i < self->swap_chain_image_count; i++) {
        image_view_create_info.image = self->swap_chain_images[i];
        vkCreateImageView(self->logical_device, &image_view_create_info, NULL, &self->swap_chain_image_views[i]);
    }
}


void _cleanup_swap_chain(struct App *self) {
    vkDeviceWaitIdle(self->logical_device);
    for (int i = 0; i < self->swap_chain_image_count; i++) { vkDestroyImageView(self->logical_device, self->swap_chain_image_views[i], NULL); }
    free(self->swap_chain_images);
    vkDestroySwapchainKHR(self->logical_device, self->swap_chain, NULL);
    self->swap_chain = NULL;
}


void _recreate_swap_chain(struct App* self) {
    int width = 0, height = 0;
    glfwGetFramebufferSize(self->window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(self->window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(self->logical_device);
    _cleanup_swap_chain(self);
    _create_swap_chain(self);
    _create_image_views(self);
}
