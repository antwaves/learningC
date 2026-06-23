#include "vulkan/vk_platform.h"
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
struct extension_info;
void _init_window(struct App* self);
void _init_vulkan(struct App* self);
void _main_loop(struct App* self);
void _clean_up(struct App* self);
void _create_instance(struct App* self);
void _get_required_instance_extensions(struct App* self);
void _setup_debug_messenger(struct App* self);
void _pick_physical_device(struct App* self);
void run(struct App* self);

void check_extensions(const char** glfw_extensions, int glfw_extension_count, bool log);
void check_validation_layers();

#define LOAD_INSTANCE_EXT(instance, func_name) PFN_##func_name func_name = (PFN_##func_name)vkGetInstanceProcAddr(instance, #func_name); 

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
    VkPhysicalDevice physical_device;
};


static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, \
                                                     VkDebugUtilsMessageTypeFlagsEXT type, \
                                                     const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, \
                                                     void* p_user_data) {
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        fprintf(stderr, "Validation Layer: type %u msg: %s\n", type, p_callback_data->pMessage);
    }
    return VK_FALSE;
}


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
    _setup_debug_messenger(self);
    _pick_physical_device(self);
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

    vkDestroyInstance(self->instance, NULL);
    glfwDestroyWindow(self->window);
    glfwTerminate();
    free(self->extensions.extension_names);
}


void _create_instance(struct App* self) {
    check_validation_layers();

    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Hello Triangle";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 1);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_4;;

    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    
    _get_required_instance_extensions(self);
    struct extension_info extension_info = self->extensions;
    check_extensions(extension_info.extension_names, extension_info.extension_count, self->log);

    create_info.enabledExtensionCount = extension_info.extension_count;
    create_info.ppEnabledExtensionNames = extension_info.extension_names;
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = NULL;
    
    if (vkCreateInstance(&create_info, NULL, &(self->instance)) != VK_SUCCESS) {
        perror("Error: ");
        exit(EXIT_FAILURE);
    }
}


void _get_required_instance_extensions(struct App* self) {
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


void _setup_debug_messenger(struct App* self) {
    if (!enable_validation_layers) return;

    PFN_vkCreateDebugUtilsMessengerEXT pfnVkCeateDebugUtilsMessengerEXT = NULL;
    LOAD_INSTANCE_EXT(self->instance, vkCreateDebugUtilsMessengerEXT);

    if (vkCreateDebugUtilsMessengerEXT == NULL) {
        fprintf(stderr, "Missing validation layers.\n");
        exit(EXIT_FAILURE);
    }

    VkDebugUtilsMessageSeverityFlagsEXT severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; 

    VkDebugUtilsMessageTypeFlagsEXT message_type_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                                                         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                                                         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | 
                                                         VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;


    VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info_ext = {0};
    debug_utils_messenger_create_info_ext.messageSeverity = severity_flags;
    debug_utils_messenger_create_info_ext.messageType = message_type_flags;
    debug_utils_messenger_create_info_ext.pfnUserCallback = &debug_callback;

    VkResult result = vkCreateDebugUtilsMessengerEXT(self->instance, &debug_utils_messenger_create_info_ext, NULL, &self->debug_messenger);
    if (result != VK_SUCCESS) {
        printf("Error: %d", VK_SUCCESS);
        exit(EXIT_FAILURE);
    }
}


void _pick_physical_device(struct App* self) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(self->instance, &device_count, NULL);

    if (device_count == 0) {
        fprintf(stderr, "Failed to find GPU's with vulkan support");
        exit(EXIT_FAILURE);
    }

    VkPhysicalDevice* devices = calloc(1, sizeof(VkPhysicalDevice) * device_count);
    vkEnumeratePhysicalDevices(self->instance, &device_count, devices);

    for (int i = 0; i < device_count; i++) {
        VkPhysicalDevice physical_device = devices[i];
        VkPhysicalDeviceProperties2* p_properties = calloc(1, sizeof(VkPhysicalDeviceProperties2));
        vkGetPhysicalDeviceProperties2(physical_device, p_properties);
        VkPhysicalDeviceFeatures2* p_features = calloc(1, sizeof(VkPhysicalDeviceFeatures2));
        vkGetPhysicalDeviceFeatures2(physical_device, p_features);

        bool supports_vulkan1_3 = p_properties->properties.apiVersion >= VK_API_VERSION_1_3;

        uint32_t count = 0;
        VkQueueFamilyProperties2* queue_family_properties;
        vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &count, queue_family_properties);
        //bool supports_graphics = 

        free(p_properties);
        free(p_features);
    }

    free(devices);
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


int main() {
    struct App* app = init();
    app->run(app);
    destroy_app(app);

    return EXIT_SUCCESS;
}
