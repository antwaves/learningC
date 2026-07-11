#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "app.h"
#include "validate.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, \
                                                     const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data);
void check_validation_layers();
void _setup_debug_messenger(struct App* self);

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, \
                                                     VkDebugUtilsMessageTypeFlagsEXT type, \
                                                     const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, \
                                                     void* p_user_data) { // debug messenger called by vulkan validation layers when it wants to give a warning. logs warnings/errors.
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        fprintf_s(stderr, "\nValidation Layer: type %u msg: %s\n\n", type, p_callback_data->pMessage);

        for (int i = 0; i < p_callback_data->objectCount; i++) {
            fprintf_s(stderr, "%s\n", p_callback_data->pObjects[i].pObjectName);
        }

        for (int i = 0; i < p_callback_data->cmdBufLabelCount; i++) {
            fprintf_s(stderr, "%s\n", p_callback_data->pCmdBufLabels[i].pLabelName);
        }

        for (int i = 0; i < p_callback_data->queueLabelCount; i++) {
            fprintf_s(stderr, "%s\n", p_callback_data->pQueueLabels[i].pLabelName);
        }
    }
    return VK_FALSE;
}


void check_validation_layers() { // checks for validation layer support from our instance
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


void _setup_debug_messenger(struct App* self) { // attach the debug callback to vulkan's validation layers
    if (!enable_validation_layers) return;

    LOAD_INSTANCE_EXT(self->instance, vkCreateDebugUtilsMessengerEXT);
    if (vkCreateDebugUtilsMessengerEXT == NULL) {
        fprintf_s(stderr, "Missing validation layers.\n");
        exit(EXIT_FAILURE);
    }

    VkDebugUtilsMessageSeverityFlagsEXT severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | // decides what types of messages get sent to the callback
                                                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT ; 
    VkDebugUtilsMessageTypeFlagsEXT message_type_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                                                         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                                                         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | 
                                                         VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
    VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info_ext = {
        .messageSeverity = severity_flags,
        .messageType = message_type_flags, 
        .pfnUserCallback = &debug_callback,
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT
    };

    VkResult result = vkCreateDebugUtilsMessengerEXT(self->instance, &debug_utils_messenger_create_info_ext, NULL, &self->debug_messenger);
    if (result != VK_SUCCESS) {
        printf("Error: %d", VK_SUCCESS);
        exit(EXIT_FAILURE);
    }
}
