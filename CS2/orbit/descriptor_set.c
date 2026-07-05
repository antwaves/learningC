#include "app.h"
#include "shader_storage_buffer.h"
#include "uniform_buffer.h"
#include "vulkan/vulkan_core.h"

#include <stdlib.h>


void _create_descriptor_set_layouts(struct App* self) {
    VkDescriptorSetLayoutBinding ssbo_layout_binding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };
    VkDescriptorSetLayoutBinding ubo_layout_binding = {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };
    VkDescriptorSetLayoutBinding bindings[2] = {ssbo_layout_binding, ubo_layout_binding};
    VkDescriptorSetLayoutCreateInfo layout_info = {
        .bindingCount = 2,
        .pBindings = bindings,
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO
    };
    vkCreateDescriptorSetLayout(self->logical_device, &layout_info, NULL, &self->descriptor_set_layout);
}


void _create_descriptor_pool(struct App* self) {
    VkDescriptorPoolSize ssbo_pool_size = { 
        .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
        .descriptorCount = self->MAX_FRAMES_IN_FLIGHT
    };
    VkDescriptorPoolSize ubo_pool_size = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = self->MAX_FRAMES_IN_FLIGHT
    };
    VkDescriptorPoolSize pool_size[2] = {ssbo_pool_size, ubo_pool_size};
    VkDescriptorPoolCreateInfo pool_info = {
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = self->MAX_FRAMES_IN_FLIGHT,
        .poolSizeCount = 2,
        .pPoolSizes = pool_size,
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO
    };
    vkCreateDescriptorPool(self->logical_device, &pool_info, NULL, &self->descriptor_pool);
}


void _create_descriptor_sets(struct App* self) {
    VkDescriptorSetLayout* layouts = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSetLayout));
    for (size_t i = 0; i < self->MAX_FRAMES_IN_FLIGHT; i++) {
       layouts[i] = self->descriptor_set_layout;
    }
    VkDescriptorSetAllocateInfo alloc_info = {
        .descriptorPool = self->descriptor_pool,
        .descriptorSetCount = self->MAX_FRAMES_IN_FLIGHT,
        .pSetLayouts = layouts,
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO
    };
    self->descriptor_sets = calloc(self->MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSet));
    vkAllocateDescriptorSets(self->logical_device, &alloc_info, self->descriptor_sets);

    for (size_t i = 0; i < self->MAX_FRAMES_IN_FLIGHT; i++) {
        if (self->shader_storage_buffers[i] == NULL) {
            printf("WARNING: No SSBO\n");
            break;
        }
        if(self->uniform_buffers[i] == NULL) {
            printf("WARNING: No UBO");
        }
        VkDescriptorBufferInfo ssbo_buffer_info = {.buffer = self->shader_storage_buffers[i], .offset = 0, .range = sizeof(struct shader_storage_object) * self->vertex_count };
        VkDescriptorBufferInfo ubo_buffer_info = {.buffer = self->uniform_buffers[i], .offset = 0, .range = sizeof(struct ProjUniformBuffer)};

        VkWriteDescriptorSet desc_writes[2] = {
            {
                .dstSet = self->descriptor_sets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pBufferInfo = &ssbo_buffer_info,
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET
            },
            {
                .dstSet = self->descriptor_sets[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &ubo_buffer_info,
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET
            }
        };
        vkUpdateDescriptorSets(self->logical_device, 2, desc_writes, 0, NULL);
    }
    
    free(layouts);
}
