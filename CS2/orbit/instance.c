#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "app.h"
#include "validate.h"

void _create_instance(struct App* self);
static void _get_required_instance_extensions(struct App* self);
static void check_extensions(const char** glfw_extensions, int glfw_extension_count, bool log);
void _create_surface(struct App* self);


void _create_instance(struct App* self) { // create the instance which connects to the vulkan library
    check_validation_layers();

    VkApplicationInfo app_info = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO, 
                                  .pApplicationName = "Hello Triangle",
                                  .applicationVersion = VK_MAKE_VERSION(1, 0, 3), 
                                  .pEngineName = "No Engine",
                                  .engineVersion = VK_MAKE_VERSION(1, 0, 3), .apiVersion = VK_API_VERSION_1_4};

    _get_required_instance_extensions(self); // glfw and physical device extensions
    struct extension_info extension_info = self->extensions;
    check_extensions(extension_info.extension_names, extension_info.extension_count, self->log);

    VkInstanceCreateInfo create_info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, 
                                        .pApplicationInfo = &app_info,
                                        .enabledExtensionCount = extension_info.extension_count, 
                                        .ppEnabledExtensionNames = extension_info.extension_names,
                                        .enabledLayerCount = 1, 
                                        .ppEnabledLayerNames = enable_validation_layers ? validation_layers : NULL
                                    };
    VkResult result = vkCreateInstance(&create_info, NULL, &(self->instance));
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create instance!");
        exit(EXIT_FAILURE);
    }
}


static void _get_required_instance_extensions(struct App* self) {  // get GLFW and validation layer extensions
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    uint32_t total_extension_count = glfw_extension_count + 1; 
    if (enable_validation_layers) {
        total_extension_count += 1;
    }

    const char** extensions = calloc(total_extension_count, sizeof(char*));
    for (uint32_t i = 0; i < glfw_extension_count; i++) {
        extensions[i] = glfw_extensions[i];
    }
    extensions[glfw_extension_count] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
    if (enable_validation_layers) {
        extensions[glfw_extension_count + 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }

    self->extensions.extension_count = total_extension_count;
    self->extensions.extension_names = extensions;
}


static void check_extensions(const char** glfw_extensions, int glfw_extension_count, bool log) { // check extension support from our instance
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL); // get num extensions
    VkExtensionProperties* extensions = malloc(extension_count * sizeof(VkExtensionProperties));
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions);

    if (log) {
        printf("%d extensions installed: \n", extension_count);
        for(int i = 0; i < extension_count; i++) {
            printf("\t%s\n", extensions[i].extensionName);
        }
    }

    if (log) {
        printf("\n%d extensions needed for GLFW, physical devices%s: \n", glfw_extension_count, enable_validation_layers ? " and validation layers" : "");
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
        fprintf_s(stderr, "Missing GLFW extensions!\n");
        exit(EXIT_FAILURE);
    }

    free(extensions);
}


void _create_surface(struct App* self) { // abstract the GLFW window into a surface, to query capabilites and present images to
    if (glfwCreateWindowSurface(self->instance, self->window, NULL, &self->surface) != 0) {
        fprintf_s(stderr, "Failed to create window surface!");
        exit(EXIT_FAILURE);
    }
}
