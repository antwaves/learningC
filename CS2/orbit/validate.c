#include "vulkan/vulkan_core.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LOAD_INSTANCE_EXT(instance, func_name) PFN_##func_name func_name = (PFN_##func_name)vkGetInstanceProcAddr(instance, #func_name); 


const char* validation_layers[] = {"VK_LAYER_KHRONOS_validation", NULL}; // MUST end with null
#ifdef NDEBUG 
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif

void check_extensions(const char** glfw_extensions, int glfw_extension_count, bool log);
void check_validation_layers();

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, \
                                                     VkDebugUtilsMessageTypeFlagsEXT type, \
                                                     const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, \
                                                     void* p_user_data) {
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        fprintf_s(stderr, "Validation Layer: type %u msg: %s\n\n", type, p_callback_data->pMessage);
    }
    return VK_FALSE;
}


void check_extensions(const char** glfw_extensions, int glfw_extension_count, bool log) { // TODO: PASS EXTENSIONS BACK OUT
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
        fprintf_s(stderr, "Missing validation layers!\n");
        exit(EXIT_FAILURE);
    }

    free(layers);
}
