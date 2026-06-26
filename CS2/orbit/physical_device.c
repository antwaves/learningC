#include "app.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void _pick_physical_device(struct App* self);
static bool is_device_suitable(VkPhysicalDevice* device);


void _pick_physical_device(struct App* self) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(self->instance, &device_count, NULL);

    if (device_count == 0) {
        fprintf_s(stderr, "Failed to find GPU's with vulkan support");
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
        fprintf_s(stderr, "Failed to find a suitable GPU");
        exit(EXIT_FAILURE);
    }

    free(devices);
}


static bool is_device_suitable(VkPhysicalDevice* device) {
    bool is_suitable;
    bool supports_vulkan1_3;
    VkPhysicalDeviceProperties2* p_properties = calloc(1, sizeof(VkPhysicalDeviceProperties2));
    p_properties->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(*device, p_properties);
    supports_vulkan1_3 = p_properties->properties.apiVersion >= VK_API_VERSION_1_3;

    bool supports_graphics = false;
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(*device, &count, NULL);
    VkQueueFamilyProperties2* queue_family_properties = calloc(count, sizeof(VkQueueFamilyProperties2));
    for (int j = 0; j < count; j++) { queue_family_properties[j].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2; }
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
