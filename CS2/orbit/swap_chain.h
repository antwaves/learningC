#ifndef SWAP_CHAIN
#define SWAP_CHAIN

#include "vulkan/vulkan_core.h"
#include "app.h"

void _create_swap_chain(struct App* self);
void _create_image_views(struct App* self);
void _create_graphics_pipeline(struct App* self);
void _recreate_swap_chain(struct App* self);
void _cleanup_swap_chain(struct App* self);

static VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR* available_formats, uint32_t items);
static VkPresentModeKHR choose_present_mode(VkPresentModeKHR* available_modes, uint32_t items);
static VkExtent2D choose_swap_extent(GLFWwindow* window, VkSurfaceCapabilitiesKHR *capabilities);uint32_t choose_swap_min_image_count(VkSurfaceCapabilitiesKHR* capabilities);
static uint32_t choose_swap_min_image_count(VkSurfaceCapabilitiesKHR* capabilities);

#endif
