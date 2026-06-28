#include <stdlib.h>
#include <stdio.h>

#include "app.h"
#include "vulkan/vulkan_core.h"

void _create_logical_device(struct App* self) { // create the logical device, which interfaces with the physical device, and allows us to send commands to it
    // search for a valid queue to use. queues are what we send commands to present images through
    uint32_t count = 0;
    bool any_queues = false;
    vkGetPhysicalDeviceQueueFamilyProperties2(self->physical_device, &count, NULL);
    VkQueueFamilyProperties2* queue_family_properties = calloc(count, sizeof(VkQueueFamilyProperties2));
    for (int i = 0; i < count; i++) { queue_family_properties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2; }
    vkGetPhysicalDeviceQueueFamilyProperties2(self->physical_device, &count, queue_family_properties);

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
        fprintf_s(stderr, "Failed to find a suitable queue");
        exit(EXIT_FAILURE);
    }
    free(queue_family_properties);

    // create our actual device
    self->queue_family_index = queue_index; // we also have to save the queue index in the list of queues, for later
    float queue_priority = 0.5f; // only one queue, so it dosen't matter
    VkDeviceQueueCreateInfo device_queue_create_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                        .queueFamilyIndex=queue_index, 
                                                        .queueCount = 1, 
                                                        .pQueuePriorities = &queue_priority};
   
    // stucture chain that enables features ( extended dynamic state, shader draw parameters, dynamic rendering and synchronization 2)
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamic_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT, .pNext = NULL, .extendedDynamicState=true}; 
    VkPhysicalDeviceVulkan11Features vulkan11_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, .pNext = &dynamic_features, .shaderDrawParameters=true};
    VkPhysicalDeviceVulkan13Features vulkan13_features = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &vulkan11_features, .dynamicRendering=true, .synchronization2=true};  
    VkPhysicalDeviceFeatures2 feature_chain = {.sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext =&vulkan13_features}; // this sucks
    const char *required_device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME};
    uint32_t ext_count = sizeof(required_device_extensions) / sizeof(char*);
    VkDeviceCreateInfo device_create_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                             .pNext = &feature_chain, 
                                             .queueCreateInfoCount = 1, 
                                             .pQueueCreateInfos = &device_queue_create_info, 
                                             .enabledExtensionCount = ext_count, 
                                             .ppEnabledExtensionNames = required_device_extensions};   
                                             
    VkResult result = vkCreateDevice(self->physical_device, &device_create_info, NULL, &self->logical_device);
    if (result != VK_SUCCESS) {
        fprintf_s(stderr, "Failed to create logical device");
        exit(EXIT_FAILURE);
    }
    vkGetDeviceQueue(self->logical_device, queue_index, 0, &self->queue);
}
