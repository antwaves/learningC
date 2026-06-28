#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vulkan/vulkan_core.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef APP
#define APP

struct extension_info {
    uint32_t extension_count;
    const char** extension_names;
};

struct App {
    uint32_t width; // window width and height
    uint32_t height;
    bool frame_buffer_resized; // whether the window has been resized
    bool log; // config, whether or not to log

    void (*run)(struct App* self); // run function supplied by user
    struct extension_info extensions;
    GLFWwindow* window;
    VkInstance instance; // connection to the vulkan library
    VkDebugUtilsMessengerEXT debug_messenger;

    VkPhysicalDevice physical_device; // our actual gpu 
    VkDevice logical_device; // vulkan's interface with the physical device
    uint32_t queue_family_index; // index of the queue in the list of queues
    VkQueue queue; // the queue that commands are presented to
    VkSurfaceKHR surface; // an abstraction of the window system

    VkSwapchainKHR swap_chain; // the infastructure that handles the queue of images that are waiting to be rendered to the screen
    VkImage* swap_chain_images; // the actual queue of images
    uint32_t swap_chain_image_count;
    VkSurfaceFormatKHR swap_chain_surface_format; //
    VkExtent2D swap_chain_extent;
    VkImageView* swap_chain_image_views;

    VkPipelineLayout pipeline_layout; 
    VkPipeline graphics_pipeline;
    VkCommandPool command_pool;

    const uint32_t MAX_FRAMES_IN_FLIGHT;
    uint32_t frame_index;
    VkCommandBuffer* command_buffers;
    VkSemaphore* present_complete_semaphores;
    VkSemaphore* render_complete_semaphores;
    VkFence* in_flight_fences;
};

#endif
