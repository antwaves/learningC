#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "app.h"
#include "instance.c"
#include "validate.c"
#include "physical_device.c"
#include "logical_device.c"
#include "swap_chain.c"
#include "descriptor_set.c"
#include "uniform_buffer.c"
#include "shader_storage_buffer.c"
#include "graphics_pipeline.c"
#include "vertex.c"
#include "command_buffer.c"
#include "draw_frame.c"


void _init_window(struct App* self) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    self->window = glfwCreateWindow(self->width, self->height, "Vulkan", NULL, NULL);
    glfwSetWindowUserPointer(self->window, self);
    glfwSetFramebufferSizeCallback(self->window, framebuffer_resize_callback);
}


void _init_vulkan(struct App* self) {
    _create_instance(self);
    _setup_debug_messenger(self);
    _create_surface(self);
    _pick_physical_device(self);
    _create_logical_device(self);
    _create_swap_chain(self);
    _create_image_views(self);
    _create_descriptor_set_layouts(self);
    _create_graphics_pipeline(self);
    _create_command_pools(self);
    _create_vertex_buffer(self);
    _create_index_buffer(self);
    _create_uniform_buffers(self);
    _create_shader_storage_buffers(self);
    _create_descriptor_pool(self);
    _create_descriptor_sets(self);
    _create_command_buffers(self);
    _create_sync_objects(self);
}


void _main_loop(struct App* self) {
    while (!glfwWindowShouldClose(self->window)) {
        glfwPollEvents();
        if (self->before_draw != NULL) {
            self->before_draw(self);
        }
        _draw_frame(self);
    }
}


void _clean_up(struct App* self) { // have to destroy things in a specific order (mostly, logical device dependent things first)
    free(self->extensions.extension_names);

    vkDeviceWaitIdle(self->logical_device);
    _cleanup_swap_chain(self);
    vkFreeMemory(self->logical_device, self->vertex_buffer_memory, NULL);
    vkFreeMemory(self->logical_device, self->index_buffer_memory, NULL);
    vkDestroyBuffer(self->logical_device, self->vertex_buffer, NULL);
    vkDestroyBuffer(self->logical_device, self->index_buffer, NULL);
    vkDestroyDescriptorSetLayout(self->logical_device, self->descriptor_set_layout, NULL);
    vkDestroyDescriptorPool(self->logical_device, self->descriptor_pool, NULL);
    for (int i = 0; i < self->MAX_FRAMES_IN_FLIGHT; i++) { // TODO: UPDATE ME ONCE ITS TIMES
        vkDestroyBuffer(self->logical_device, self->shader_storage_buffers[i], NULL);
        vkDestroyBuffer(self->logical_device, self->uniform_buffers[i], NULL);
        vkFreeMemory(self->logical_device, self->shader_storage_buffers_memory[i], NULL);
        vkFreeMemory(self->logical_device, self->uniform_buffers_memory[i], NULL);
    }
    
    free(self->shader_storage_buffers);
    free(self->shader_storage_buffers_memory);
    free(self->shader_storage_buffers_mapped);
    free(self->uniform_buffers);
    free(self->uniform_buffers_memory);
    free(self->uniform_buffers_mapped);
    free(self->descriptor_sets);
    vkDestroyPipelineLayout(self->logical_device, self->pipeline_layout, NULL);
    vkDestroyPipeline(self->logical_device, self->graphics_pipeline, NULL);
    free(self->command_buffers);
    
    vkDestroyCommandPool(self->logical_device, self->graphics_command_pool, NULL);
    vkDestroyCommandPool(self->logical_device, self->transfer_command_pool, NULL);

    for (size_t i = 0; i < self->MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(self->logical_device, self->present_complete_semaphores[i], NULL);
        vkDestroyFence(self->logical_device, self->in_flight_fences[i], NULL);
    }
    free(self->present_complete_semaphores);
    free(self->in_flight_fences);

    for (size_t i = 0; i < self->swap_chain_image_count; i++) {
        vkDestroySemaphore(self->logical_device, self->render_complete_semaphores[i], NULL);
    }
    free(self->render_complete_semaphores);
    
    vkDestroySurfaceKHR(self->instance, self->surface, NULL);
    LOAD_INSTANCE_EXT(self->instance, vkDestroyDebugUtilsMessengerEXT)
    if (vkDestroyDebugUtilsMessengerEXT != NULL) {
        vkDestroyDebugUtilsMessengerEXT(self->instance, self->debug_messenger, NULL);
    }

    vkDestroyDevice(self->logical_device, NULL);
    vkDestroyInstance(self->instance, NULL);
    glfwDestroyWindow(self->window);
    glfwTerminate();
}


void run(struct App* self) {
    _init_window(self);
    if (self->before_initialization != NULL) {
        self->before_initialization(self);
    }
    _init_vulkan(self);
    _main_loop(self);
    _clean_up(self);
}


struct App* init(void (*user_before_initialization)(struct App* self), void (*user_before_draw)(struct App* self)) {
    struct App app = {
        .run = run,
        .width = 800,
        .height = 800,
        .log = false,
        .MAX_FRAMES_IN_FLIGHT = 2,
        .before_initialization = user_before_initialization,
        .before_draw = user_before_draw,
    };
    struct App* a = calloc(1, sizeof(app));
    memcpy(a, &app, sizeof(app));
    return a;
}


void destroy_app(struct App* a) {
    free(a);
}
