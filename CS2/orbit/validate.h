#ifndef VALIDATE
#define VALIDATE

#include "vulkan/vulkan_core.h"
#include <stdbool.h>
#include "app.h"

#define LOAD_INSTANCE_EXT(instance, func_name) PFN_##func_name func_name = (PFN_##func_name)vkGetInstanceProcAddr(instance, #func_name); 

const char* validation_layers[] = {"VK_LAYER_KHRONOS_validation", NULL}; // MUST end with null
#ifdef NDEBUG 
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, \
                                                     const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data);

void check_validation_layers();
void _setup_debug_messenger(struct App* self);

#endif
