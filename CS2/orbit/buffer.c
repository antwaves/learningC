#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "vulkan/vulkan_core.h"


void create_buffer(VkBuffer buffer, VkDeviceMemory buffer_memory, VkDevice logical_device, VkPhysicalDevice physical_device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkBufferCreateInfo buffer_info = {
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_CONCURRENT,
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO
    };
    vkCreateBuffer(logical_device, &buffer_info, NULL, &buffer);

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(logical_device, buffer, &mem_requirements);
    VkMemoryAllocateInfo alloc_info = {
        .allocationSize = mem_requirements.size, 
        .memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties, &physical_device),
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO
    };
    vkAllocateMemory(logical_device, &alloc_info, NULL, &buffer_memory);
    vkBindBufferMemory(logical_device, buffer, buffer_memory, 0);
}


uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties, VkPhysicalDevice* physical_device) {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(*physical_device, &mem_properties);

    for (uint32_t i = 0; i <  mem_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    fprintf(stderr, "Failed to find suitable memory type!");
    exit(EXIT_FAILURE);
}


void copy_buffer(VkCommandPool command_pool, VkQueue transfer_queue, VkDevice logical_device,  VkBuffer src_buffer, VkBuffer dst_buffer, int size) {
    VkCommandBufferAllocateInfo alloc_info = {
        .commandPool = command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    VkCommandBuffer cmd_buffer;
    vkAllocateCommandBuffers(logical_device, &alloc_info, &cmd_buffer);
    VkCommandBufferBeginInfo vk_command_buffer_begin_info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(cmd_buffer, &vk_command_buffer_begin_info); 
    VkBufferCopy region = {.srcOffset = 0, .dstOffset = 0, .size = size};
    vkCmdCopyBuffer(cmd_buffer, src_buffer, dst_buffer, 1, &region);
    vkEndCommandBuffer(cmd_buffer);
    
    const VkSubmitInfo submit_info = {
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd_buffer, 
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO
    };

    vkQueueSubmit(transfer_queue, 1, &submit_info, NULL);
    vkQueueWaitIdle(transfer_queue);
}
