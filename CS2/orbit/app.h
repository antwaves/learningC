#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <stdint.h>
#include <stdbool.h>
#include "vertex.h"

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

    const uint32_t MAX_FRAMES_IN_FLIGHT; // max amount of frames processed at once
    uint32_t frame_index;

    void (*run)(struct App* self); 
    struct extension_info extensions; // instance extensions
    GLFWwindow* window;
    VkInstance instance; // connection to the vulkan library
    VkDebugUtilsMessengerEXT debug_messenger;

    VkPhysicalDevice physical_device; // repersentation our actual gpu 
    VkDevice logical_device; // vulkan's interface with the physical device
    uint32_t graphics_queue_family_index;
    uint32_t transfer_queue_family_index;
    VkQueue graphics_queue; 
    VkQueue transfer_queue; 
    VkSurfaceKHR surface; // an abstraction of the window system

    VkSwapchainKHR swap_chain; // the infastructure that handles the queue of images that are waiting to be rendered to the screen
    VkImage* swap_chain_images; // the actual queue of images
    uint32_t swap_chain_image_count;
    VkSurfaceFormatKHR swap_chain_surface_format;
    VkExtent2D swap_chain_extent; 
    VkImageView* swap_chain_image_views; // description of how swapchain images should be interpreted

    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorPool descriptor_pool;
    VkDescriptorSet* descriptor_sets;
    VkPipelineLayout pipeline_layout; // describe uniforms in the pipeline
    VkPipeline graphics_pipeline; // describes how to present our vertices to the screen

    Vertex* vertices;
    uint32_t vertex_count;
    uint16_t* indices;
    uint64_t index_count;
    
    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;
    VkBuffer index_buffer;
    VkDeviceMemory index_buffer_memory;

    VkBuffer* shader_storage_buffers;
    VkDeviceMemory* shader_storage_buffers_memory;
    void** shader_storage_buffers_mapped;

    VkCommandPool graphics_command_pool; 
    VkCommandPool transfer_command_pool;
    VkCommandBuffer* command_buffers; // array of command buffers. commands are stored, then sent all at once.

    VkSemaphore* present_complete_semaphores;  // makes the GPU wait for the presentation engine to stop using a image
    VkSemaphore* render_complete_semaphores; //  makes the GPU wait to present an image until it's finished rendering
    VkFence* in_flight_fences;  // makes the CPU wait to process the next image until the last one has finished presenting
};


void run(struct App* self);
struct App* init();
void destroy_app(struct App* a);

#endif
