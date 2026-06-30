#include "app.h"

#ifndef BUFFER_OPERATIONS
#define BUFFER_OPERATIONS
uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties, VkPhysicalDevice* physical_device);
void create_buffer(VkBuffer* p_buffer, VkDeviceMemory* p_buffer_memory, struct App* app, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, struct App* app, int size);
#endif
