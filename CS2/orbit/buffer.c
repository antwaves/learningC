#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "vulkan/vulkan_core.h"


bool create_buffer(VkBuffer* p_buffer, VkDeviceMemory* p_buffer_memory, struct App* app, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) { // creates a vulkan buffer at p_buffer, binding the memory to p_buffer memory
    if (size <= 0) {
        return false;
    }
    bool does_transfer_queue_not_exist = app->graphics_queue_family_index == app->transfer_queue_family_index;
    uint32_t queue_family_indices[] = {app->graphics_queue_family_index, app->transfer_queue_family_index};
    uint32_t num_queue_family_indices = 1 + !does_transfer_queue_not_exist;
    int sharing_mode = does_transfer_queue_not_exist ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

    VkBufferCreateInfo buffer_info = {
        .size = size,
        .usage = usage,
        .sharingMode = sharing_mode,
        .queueFamilyIndexCount = num_queue_family_indices,
        .pQueueFamilyIndices = queue_family_indices,
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO
    };
    vkCreateBuffer(app->logical_device, &buffer_info, NULL, p_buffer);

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(app->logical_device, *p_buffer, &mem_requirements);
    VkMemoryAllocateInfo alloc_info = {
        .allocationSize = mem_requirements.size, 
        .memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties, &app->physical_device),
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO
    };
    vkAllocateMemory(app->logical_device, &alloc_info, NULL, p_buffer_memory);
    vkBindBufferMemory(app->logical_device, *p_buffer, *p_buffer_memory, 0);
    return true;
}


uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties, VkPhysicalDevice* physical_device) { // finds the index of a preffered memory type in a memory proeperties arr of a physical device
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


void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, struct App* app, int size) { // copies a buffer from src_buffer to dst_buffer using a temporary command buffer. submits to transfer queue
    VkCommandBufferAllocateInfo alloc_info = {
        .commandPool = app->transfer_command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO
    };
    VkCommandBuffer cmd_buffer;
    vkAllocateCommandBuffers(app->logical_device, &alloc_info, &cmd_buffer);

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
    vkQueueSubmit(app->transfer_queue, 1, &submit_info, NULL); // if there is no transfer queue, we expect that the app has designated another queue as one
    vkQueueWaitIdle(app->transfer_queue);
}
