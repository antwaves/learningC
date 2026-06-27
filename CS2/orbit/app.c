#include "vulkan/vulkan_core.h"
#include "app.h"
#include "validate.c"
#include "instance.c"
#include "physical_device.c"
#include "logical_device.c"
#include "swap_chain.c"
#include "graphics_pipeline.c"
#include "command_buffer.c"
#include "draw_frame.c"


void _init_window(struct App* self);
void _init_vulkan(struct App* self);
void _main_loop(struct App* self);
void _clean_up(struct App* self);

void run(struct App* self);
struct App* init();
void destroy_app(struct App* a);


void _init_window(struct App* self) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    self->window = glfwCreateWindow(self->WIDTH, self->HEIGHT, "Vulkan", NULL, NULL);
}


void _init_vulkan(struct App* self) {
    _create_instance(self);
    _setup_debug_messenger(self);
    _create_surface(self);
    _pick_physical_device(self);
    _create_logical_device(self);
    _create_swap_chain(self);
    _create_image_views(self);
    _create_graphics_pipeline(self);
    _create_command_pool(self);
    _create_command_buffer(self);
    _create_sync_objects(self);
}


void _main_loop(struct App* self) {
    while (!glfwWindowShouldClose(self->window)) {
        glfwPollEvents();
        _draw_frame(self);
    }
}


void _clean_up(struct App* self) {
    free(self->extensions.extension_names);

    vkDeviceWaitIdle(self->logical_device);
    for (int i = 0; i < self->swap_chain_image_count; i++) { vkDestroyImageView(self->logical_device, self->swap_chain_image_views[i], NULL); }
    free(self->swap_chain_images);
    vkDestroySwapchainKHR(self->logical_device, self->swap_chain, NULL);
    vkDestroyPipelineLayout(self->logical_device, self->pipeline_layout, NULL);
    vkDestroyPipeline(self->logical_device, self->graphics_pipeline, NULL);
    vkDestroyCommandPool(self->logical_device, self->command_pool, NULL);
    vkDestroySemaphore(self->logical_device, self->present_complete_semaphore, NULL);
    vkDestroySemaphore(self->logical_device, self->render_complete_semaphore, NULL);
    vkDestroyFence(self->logical_device, self->draw_fence, NULL);
    
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
    _init_vulkan(self);
    _main_loop(self);
    _clean_up(self);
}


struct App* init() {
    struct App* a = calloc(1, sizeof(struct App));
    a->run = run;
    a->WIDTH = 800;
    a->HEIGHT = 600;
    a->log = true;
    return a;
}


void destroy_app(struct App* a) {
    free(a);
}
