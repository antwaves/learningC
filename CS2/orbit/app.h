#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdbool.h>


#ifndef APP
#define APP

struct extension_info {
    uint32_t extension_count;
    const char** extension_names;
};

struct App {
    uint32_t WIDTH;
    uint32_t HEIGHT;
    bool log;

    void (*run)(struct App* self);
    struct extension_info extensions;
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;

    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    uint32_t queue_family_index;
    VkQueue queue;

    VkSwapchainKHR swap_chain;
    VkImage* swap_chain_images;
    uint32_t swap_chain_image_count;
    VkSurfaceFormatKHR swap_chain_surface_format;
    VkExtent2D swap_chain_extent;
    VkImageView* swap_chain_image_views;

    VkPipelineLayout pipeline_layout; 
    VkPipeline graphics_pipeline;
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
};

#endif