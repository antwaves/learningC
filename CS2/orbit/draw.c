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
void _create_surface(struct App* self);
void _pick_physical_device(struct App* self);
void _create_logical_device(struct App* self);
void _create_swap_chain(struct App* self);
void run(struct App* self);

void check_extensions(const char** glfw_extensions, int glfw_extension_count, bool log);
void check_validation_layers();
bool is_device_suitable(VkPhysicalDevice* device);
VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR* available_formats, uint32_t items);
VkPresentModeKHR choose_present_mode(VkPresentModeKHR* available_modes, uint32_t items);
VkExtent2D choose_swap_extent(GLFWwindow* window, VkSurfaceCapabilitiesKHR *capabilities);uint32_t choose_swap_min_image_count(VkSurfaceCapabilitiesKHR* capabilities);
uint32_t choose_swap_min_image_count(VkSurfaceCapabilitiesKHR* capabilities);

#define LOAD_INSTANCE_EXT(instance, func_name) PFN_##func_name func_name = (PFN_##func_name)vkGetInstanceProcAddr(instance, #func_name); 
#define clamp(d, min, max) (d < min ? min : d) > max ? max : (d < min ? min : d)

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
    VkQueue queue;
    VkSwapchainKHR swap_chain;
    VkImage* swap_chain_images;
    VkSurfaceFormatKHR swap_chain_surface_format;
    VkExtent2D swap_chain_extent;
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
    _create_surface(self);
    _pick_physical_device(self);
    _create_logical_device(self);
    _create_swap_chain(self);
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
    vkDestroySwapchainKHR(self->logical_device, self->swap_chain, NULL);
    vkDestroyDevice(self->logical_device, NULL);
    vkDestroyInstance(self->instance, NULL);
    glfwDestroyWindow(self->window);
    glfwTerminate();
    free(self->extensions.extension_names);
}


void _create_instance(struct App* self) {
    check_validation_layers();

    VkApplicationInfo app_info = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO, 
                                  .pApplicationName = "Hello Triangle",
                                  .applicationVersion = VK_MAKE_VERSION(1, 0, 1), 
                                  .pEngineName = "No Engine",
                                  .engineVersion = VK_MAKE_VERSION(1, 0, 0), .apiVersion = VK_API_VERSION_1_4};

    _get_required_instance_extensions(self);
    struct extension_info extension_info = self->extensions;
    check_extensions(extension_info.extension_names, extension_info.extension_count, self->log);
    VkInstanceCreateInfo create_info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, 
                                        .pApplicationInfo = &app_info,
                                        .enabledExtensionCount = extension_info.extension_count, 
                                        .ppEnabledExtensionNames = extension_info.extension_names,
                                        .enabledLayerCount = 0, 
                                        .ppEnabledLayerNames = NULL};
    
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
                                                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT ; 

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


void _create_surface(struct App* self) {
    if (glfwCreateWindowSurface(self->instance, self->window, NULL, &self->surface) != 0) {
        fprintf(stderr, "Failed to create window surface!");
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
        if (is_device_suitable(&physical_device)) {
            self->physical_device = physical_device;
        }
    }

    if (self->physical_device == NULL) {
        fprintf(stderr, "Failed to find a suitable GPU");
        exit(EXIT_FAILURE);
    }

    free(devices);
}


bool is_device_suitable(VkPhysicalDevice* device) {
    bool is_suitable;
    bool supports_vulkan1_3;
    VkPhysicalDeviceProperties2* p_properties = calloc(1, sizeof(VkPhysicalDeviceProperties2));
    vkGetPhysicalDeviceProperties2(*device, p_properties);
    supports_vulkan1_3 = p_properties->properties.apiVersion >= VK_API_VERSION_1_3;

    bool supports_graphics = false;
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(*device, &count, NULL);
    VkQueueFamilyProperties2* queue_family_properties = calloc(1, count * sizeof(VkQueueFamilyProperties2));
    vkGetPhysicalDeviceQueueFamilyProperties2(*device, &count, queue_family_properties);
    for (int j = 0; j < count; j++) {
        supports_graphics = supports_graphics || queue_family_properties[j].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    }

    bool has_needed_extensions = true;
    char *required_device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    uint32_t elms = sizeof(required_device_extensions) / sizeof(char*);
    uint32_t available_ext_count = 0;
    vkEnumerateDeviceExtensionProperties(*device, NULL, &available_ext_count, NULL);
    VkExtensionProperties* available_device_extensions = calloc(1, sizeof(VkExtensionProperties) * available_ext_count);
    vkEnumerateDeviceExtensionProperties(*device, NULL, &available_ext_count, available_device_extensions);
    for (int i = 0; i < elms; i++) {
        bool has_extension = false;
        for (int j = 0; j < available_ext_count; j++) {
            has_extension = has_extension || strcmp(available_device_extensions[j].extensionName, required_device_extensions[i]) == 0;
        }
        has_needed_extensions = has_needed_extensions && has_extension;
    }

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamic_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT, .pNext = NULL, .extendedDynamicState=true};
    VkPhysicalDeviceVulkan11Features vulkan11_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, .pNext = &dynamic_features, .shaderDrawParameters=true};
    VkPhysicalDeviceVulkan13Features vulkan13_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &vulkan11_features, .dynamicRendering=true};  
    VkPhysicalDeviceFeatures2 feature_chain = {.sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext =&vulkan13_features};
    vkGetPhysicalDeviceFeatures2(*device, &feature_chain);
    bool has_needed_features = vulkan11_features.shaderDrawParameters && vulkan13_features.dynamicRendering && dynamic_features.extendedDynamicState;

    is_suitable = supports_vulkan1_3 && supports_graphics && has_needed_extensions && has_needed_features;

    free(p_properties);
    free(queue_family_properties);
    free(available_device_extensions);

    return is_suitable;
}


void _create_logical_device(struct App* self) {
    VkPhysicalDevice device = self->physical_device;
    uint32_t count = 0;

    bool any_queues = false;
    vkGetPhysicalDeviceQueueFamilyProperties2(device, &count, NULL);
    VkQueueFamilyProperties2* queue_family_properties = calloc(1, count * sizeof(VkQueueFamilyProperties2));
    vkGetPhysicalDeviceQueueFamilyProperties2(device, &count, queue_family_properties);
    uint32_t queue_index = ~0;
    for (int i = 0; i < count; i++) {
        bool graphics_supported = queue_family_properties[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        VkBool32 surface_supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(self->physical_device, i, self->surface, &surface_supported);
        if (graphics_supported && surface_supported) {
            queue_index = i; 
            any_queues = true;
            break;
        }
    }
    if (!any_queues) {
        fprintf(stderr, "Failed to find a suitable queue");
        exit(EXIT_FAILURE);
    }

    float queue_priority = 0.5f;
    VkDeviceQueueCreateInfo device_queue_create_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                        .queueFamilyIndex=queue_index, 
                                                        .queueCount = 1, 
                                                        .pQueuePriorities = &queue_priority};
   
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamic_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT, .pNext = NULL}; 
    VkPhysicalDeviceVulkan11Features vulkan11_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, .pNext = &dynamic_features};
    VkPhysicalDeviceVulkan13Features vulkan13_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &vulkan11_features};  
    VkPhysicalDeviceFeatures2 feature_chain = {.sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext =&vulkan13_features}; // this sucks

    const char *required_device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    uint32_t ext_count = sizeof(required_device_extensions) / sizeof(char*);
    VkDeviceCreateInfo device_create_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                             .pNext = &feature_chain, 
                                             .queueCreateInfoCount = 1, 
                                             .pQueueCreateInfos = &device_queue_create_info, 
                                             .enabledExtensionCount = ext_count, 
                                             .ppEnabledExtensionNames = required_device_extensions};    
    if (vkCreateDevice(self->physical_device, &device_create_info, NULL, &self->logical_device) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create logical device");
        exit(EXIT_FAILURE);
    }
    vkGetDeviceQueue(self->logical_device, queue_index, 0, &self->queue);
}


void _create_swap_chain(struct App* self) {
    VkSurfaceCapabilitiesKHR* surface_capabilities = calloc(1, sizeof(VkSurfaceCapabilitiesKHR));
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(self->physical_device, self->surface, surface_capabilities);
    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(self->physical_device, self->surface, &format_count, NULL);
    VkSurfaceFormatKHR* available_formats = calloc(format_count, sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(self->physical_device, self->surface, &format_count, available_formats);
    uint32_t mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(self->physical_device, self->surface, &mode_count, NULL);
    VkPresentModeKHR* available_present_modes = calloc(mode_count, sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(self->physical_device, self->surface, &mode_count, available_present_modes);

    VkSurfaceFormatKHR swap_format = choose_swap_surface_format(available_formats, format_count);
    VkPresentModeKHR present_mode = choose_present_mode(available_present_modes, mode_count);
    VkExtent2D swap_extent = choose_swap_extent(self->window, surface_capabilities);
    uint32_t min_image_count = choose_swap_min_image_count(surface_capabilities);

    VkSwapchainCreateInfoKHR swap_chain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = self->surface,
        .minImageCount = min_image_count,
        .imageFormat = swap_format.format,
        .imageColorSpace = swap_format.colorSpace,
        .imageExtent = swap_extent, 
        .imageArrayLayers = 1,
        .imageUsage =  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = surface_capabilities->currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = true,
    };
    swap_chain_info.oldSwapchain = NULL;
    
    if (vkCreateSwapchainKHR(self->logical_device, &swap_chain_info, NULL, &self->swap_chain) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create swap_chain!");
        exit(EXIT_FAILURE);
    }

    uint32_t image_count = 0;
    vkGetSwapchainImagesKHR(self->logical_device, self->swap_chain, &image_count, self->swap_chain_images);
    self->swap_chain_surface_format = swap_format;
    self->swap_chain_extent = swap_extent;

    free(available_formats);
    free(surface_capabilities);
    free(available_present_modes);
}



VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR* available_formats, uint32_t items) {
    VkSurfaceFormatKHR* format_preferred = NULL;
    for (int i = 0; i < items; i++) {
        VkSurfaceFormatKHR f = available_formats[i];
        if (f.format == VK_FORMAT_R8G8B8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)  {
            format_preferred = &f;
            break;
        }
    }
    
    return format_preferred == NULL ? available_formats[0] : *format_preferred;
}


VkPresentModeKHR choose_present_mode(VkPresentModeKHR* available_modes, uint32_t items) {
    VkPresentModeKHR* mode_preferred = NULL;
    for (int i = 0; i < items; i++) {
        if (available_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            mode_preferred = &available_modes[i];
        }
    }
    return mode_preferred == NULL ? VK_PRESENT_MODE_FIFO_KHR : *mode_preferred;
}


VkExtent2D choose_swap_extent(GLFWwindow* window, VkSurfaceCapabilitiesKHR *capabilities) {
    VkSurfaceCapabilitiesKHR c = *capabilities;
    if (c.currentExtent.width !=  UINT32_MAX) {
        return c.currentExtent;
    }
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    VkExtent2D extent = {clamp(width, c.minImageExtent.width, c.maxImageExtent.width), 
                         clamp(height, c.minImageExtent.height, c.maxImageExtent.width)};
    return extent;
}


uint32_t choose_swap_min_image_count(VkSurfaceCapabilitiesKHR* capabilities) {
    uint32_t min_image_count = max(3u, capabilities->minImageCount);
    if ((0 < capabilities->maxImageCount) && capabilities->maxImageCount < min_image_count) {
        min_image_count = capabilities->maxImageCount;
    }
    return min_image_count;
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
