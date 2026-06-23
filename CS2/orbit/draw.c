#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// debug stuff
const char* validation_layers[] = {"VK_LAYER_KHRONOS_validation", NULL}; // MUST end with null
#ifdef NDEBUG 
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif

// not debug stuff
struct App;
void _init_window(struct App* self);
void _init_vulkan(struct App* self);
void _main_loop(struct App* self);
void _clean_up(struct App* self);
void check_extensions(const char** glfw_extensions, int glfw_extension_count, bool log);
void check_validation_layers();
void _create_instance(struct App* self);
void run(struct App* self);

struct App{
    uint32_t WIDTH;
    uint32_t HEIGHT;
    bool log;

    void (*run)(struct App* self);
    GLFWwindow* window;
    VkInstance instance;
};


void run(struct App* self) {
    _init_window(self);
    _init_vulkan(self);
    _main_loop(self);
    _clean_up(self);
}


void _init_window(struct App* self) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    self->window = glfwCreateWindow(self->WIDTH, self->HEIGHT, "Vulkan", NULL, NULL);
}


void _init_vulkan(struct App* self) {
    _create_instance(self);
}


void _main_loop(struct App* self) {
    while (!glfwWindowShouldClose(self->window)) {
        glfwPollEvents();
    }
}


void _clean_up(struct App* self) {
    vkDestroyInstance(self->instance, NULL);
    glfwDestroyWindow(self->window);
    glfwTerminate();
}


void _create_instance(struct App* self) {
    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Hello Triangle";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_4;;

    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = NULL;

    check_extensions(glfw_extensions, glfw_extension_count, self->log);
    check_validation_layers();
    
    if (vkCreateInstance(&create_info, NULL, &(self->instance)) != VK_SUCCESS) {
        perror("Error: ");
        exit(EXIT_FAILURE);
    }
}


void check_extensions(const char** glfw_extensions, int glfw_extension_count, bool log) {
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);

    VkExtensionProperties* extensions = malloc(extension_count * sizeof(VkExtensionProperties));
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions);

    if (log) {
        printf("%d extensions installed: \n", extension_count);
        for(int i = 0; i < extension_count; i++) {
            printf("\t%s\n", extensions[i].extensionName);
        }
    }

    if (log) {
        printf("\n%d extensions needed for GLFW: \n", glfw_extension_count);
    }
    bool missing_any = false;
    for (int i = 0; i < glfw_extension_count; i++) {
        bool in_extensions = false;

        for (int j = 0; j < extension_count; j++) {
            in_extensions = in_extensions || strcmp(glfw_extensions[i], extensions[j].extensionName) == 0;
        }

        if (log) {
            printf("\t%s: %s\n", glfw_extensions[i], (in_extensions ? "Installed" : "Missing"));

        }
        missing_any = missing_any || !in_extensions;
    }

    if (missing_any) {
        fprintf(stderr, "Missing GLFW extensions!\n");
        exit(EXIT_FAILURE);
    }

    free(extensions);
}


void check_validation_layers() {
    VkLayerProperties* layers;
    uint32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    layers = malloc(sizeof(VkLayerProperties) * layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layers);

    bool contains_all = true;
    const char** required_layers = validation_layers;
    while (*required_layers != NULL) {
        bool contains = false;
        for (int i = 0; i < layer_count; i++) {
            contains = contains || strcmp(layers[i].layerName, *required_layers) == 0;
        }

        required_layers++;
        contains_all = contains_all && contains;
    }

    if (!contains_all) {
        fprintf(stderr, "Missing validation layers!\n");
        exit(EXIT_FAILURE);
    }

    free(layers);
}

struct App* init() {
    struct App* a = malloc(sizeof(struct App));
    a->run = run;
    a->WIDTH = 800;
    a->HEIGHT = 600;
    a->log = true;
    return a;
}


void destroy_app(struct App* a) {
    free(a);
}


int main() {
    struct App* app = init();
    app->run(app);
    destroy_app(app);

    return EXIT_SUCCESS;
}
