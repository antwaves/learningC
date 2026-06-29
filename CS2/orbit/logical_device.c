#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <xkeycheck.h>

#include "app.h"
#include "vulkan/vulkan_core.h"

bool get_graphics_queue_index(VkQueueFamilyProperties2* queue_family_properties, uint32_t queue_family_count, uint32_t* index, struct App* self);
bool get_transfer_queue_index(VkQueueFamilyProperties2* queue_family_properties, uint32_t queue_family_count, uint32_t* p_index);


void _create_logical_device(struct App* self) { // create the logical device, which interfaces with the physical device, and allows us to send commands to it
    // search for a valid queue to use. 
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(self->physical_device, &count, NULL);
    VkQueueFamilyProperties2* queue_family_properties = calloc(count, sizeof(VkQueueFamilyProperties2));
    for (int i = 0; i < count; i++) { queue_family_properties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2; }
    vkGetPhysicalDeviceQueueFamilyProperties2(self->physical_device, &count, queue_family_properties);

    uint32_t g_queue_index = ~0;
    if (!get_graphics_queue_index(queue_family_properties, count, &g_queue_index, self)) {
        fprintf_s(stderr, "Failed to find a suitable queue");
        exit(EXIT_FAILURE);
    }
    uint32_t t_queue_index = ~0;
    bool transfer_queue_exists = get_transfer_queue_index(queue_family_properties, count, &t_queue_index);
    if (!transfer_queue_exists) {
        fprintf_s(stderr, "Failed to find a transfer queue, defaulting");
        t_queue_index = g_queue_index;
    }

    free(queue_family_properties);

    // create our actual device
    VkDeviceQueueCreateInfo *device_queues_create_infos = calloc(2, sizeof(VkDeviceQueueCreateInfo));
    uint32_t queue_count = 1;

    self->graphics_queue_family_index = g_queue_index;
    self->transfer_queue_family_index = t_queue_index;
    float g_queue_priority = 1.0f, t_queue_priority = 0.5f;
    VkDeviceQueueCreateInfo graphics_device_queue_create_info = {
        .queueFamilyIndex=g_queue_index, 
        .queueCount = 1, 
        .pQueuePriorities = &g_queue_priority,
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO
    };
    device_queues_create_infos[0] = graphics_device_queue_create_info;

    if (transfer_queue_exists) {
        VkDeviceQueueCreateInfo transfer_device_queue_create_info = {
            .queueFamilyIndex=t_queue_index, 
            .queueCount = 1, 
            .pQueuePriorities = &t_queue_priority,
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO
        };
        device_queues_create_infos[1] = transfer_device_queue_create_info;
        queue_count = 2;
    }
    else {
        VkDeviceQueueCreateInfo *oldptr = device_queues_create_infos;
        free(oldptr);
        device_queues_create_infos = &graphics_device_queue_create_info;
    }
    

    // stucture chain that enables features ( extended dynamic state, shader draw parameters, dynamic rendering and synchronization 2)
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamic_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT, .pNext = NULL, .extendedDynamicState=true}; 
    VkPhysicalDeviceVulkan11Features vulkan11_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, .pNext = &dynamic_features, .shaderDrawParameters=true};
    VkPhysicalDeviceVulkan13Features vulkan13_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &vulkan11_features, .dynamicRendering=true, .synchronization2=true};  
    VkPhysicalDeviceFeatures2 feature_chain = {.sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext =&vulkan13_features}; // this sucks
    const char *required_device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME};
    uint32_t ext_count = sizeof(required_device_extensions) / sizeof(char*);
    VkDeviceCreateInfo device_create_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                             .pNext = &feature_chain, 
                                             .queueCreateInfoCount = queue_count, 
                                             .pQueueCreateInfos = device_queues_create_infos, 
                                             .enabledExtensionCount = ext_count, 
                                             .ppEnabledExtensionNames = required_device_extensions};   
                                             
    VkResult result = vkCreateDevice(self->physical_device, &device_create_info, NULL, &self->logical_device);
    if (result != VK_SUCCESS) {
        fprintf_s(stderr, "Failed to create logical device");
        exit(EXIT_FAILURE);
    }
    vkGetDeviceQueue(self->logical_device, g_queue_index, 0, &self->graphics_queue);
    vkGetDeviceQueue(self->logical_device, t_queue_index, 0, &self->transfer_queue);
    free(device_queues_create_infos);
}


bool get_graphics_queue_index(VkQueueFamilyProperties2* queue_family_properties, uint32_t queue_family_count, uint32_t* p_index, struct App* self) {
    for (int i = 0; i < queue_family_count; i++) {
        bool graphics_supported = queue_family_properties[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        VkBool32 surface_supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(self->physical_device, i, self->surface, &surface_supported);
        if (graphics_supported && surface_supported) {
            *p_index = i; 
            return true;
        }
    }
    return false;
}


bool get_transfer_queue_index(VkQueueFamilyProperties2* queue_family_properties, uint32_t queue_family_count, uint32_t* p_index) {
    for (int i = 0; i < queue_family_count; i++) {
        int queue_flags = queue_family_properties[i].queueFamilyProperties.queueFlags;
        bool graphics_supported = queue_flags & VK_QUEUE_GRAPHICS_BIT;
        bool transfer_supported = queue_flags & VK_QUEUE_TRANSFER_BIT;
        if (!graphics_supported && transfer_supported) {
            *p_index = i; 
            return true;
        }
    }
    return false;
}
