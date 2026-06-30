#include "app.h"

#ifndef BUFFER_OPERATIONS
#define BUFFER_OPERATIONS
uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties, VkPhysicalDevice* physical_device);
void create_buffer(VkBuffer buffer, VkDeviceMemory buffer_memory, VkDevice logical_device, VkPhysicalDevice physical_device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
void copy_buffer(VkCommandPool command_pool, VkQueue transfer_queue, VkDevice logical_device,  VkBuffer src_buffer, VkBuffer dst_buffer, int size);
#endif
