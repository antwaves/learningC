#include "app.h"
#include "validate.c"
#include "instance.c"
#include "physical_device.c"
#include "logical_device.c"
#include "swap_chain.c"
#include "graphics_pipeline.c"

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
}


void _main_loop(struct App* self) {
    while (!glfwWindowShouldClose(self->window)) {
        glfwPollEvents();
    }
}


void _clean_up(struct App* self) {
    LOAD_INSTANCE_EXT(self->instance, vkDestroyDebugUtilsMessengerEXT)
    if (vkDestroyDebugUtilsMessengerEXT != NULL) {
        vkDestroyDebugUtilsMessengerEXT(self->instance, self->debug_messenger, NULL);
    }

    vkDeviceWaitIdle(self->logical_device);
    for (int i = 0; i < self->swap_chain_image_count; i++) { vkDestroyImageView(self->logical_device, self->swap_chain_image_views[i], NULL); }
    free(self->swap_chain_images);
    vkDestroySwapchainKHR(self->logical_device, self->swap_chain, NULL);
    vkDestroySurfaceKHR(self->instance, self->surface, NULL);
    vkDestroyDevice(self->logical_device, NULL);
    vkDestroyInstance(self->instance, NULL);
    glfwDestroyWindow(self->window);
    glfwTerminate();
    free(self->extensions.extension_names);
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
