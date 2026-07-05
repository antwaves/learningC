#include "app.h"
#include "shader_storage_buffer.h"

#include <stdlib.h>


void _create_descriptor_set_layout(struct App* self) {
    VkDescriptorSetLayoutBinding ssbo_layout_binding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };
    VkDescriptorSetLayoutCreateInfo layout_info = {
        .bindingCount = 1,
        .pBindings = &ssbo_layout_binding,
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO
    };
    vkCreateDescriptorSetLayout(self->logical_device, &layout_info, NULL, &self->descriptor_set_layout);
}


void _create_descriptor_pool(struct App* self) {
    VkDescriptorPoolSize pool_size = { 
        .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
        .descriptorCount = self->MAX_FRAMES_IN_FLIGHT
    };
    VkDescriptorPoolCreateInfo pool_info = {
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = self->MAX_FRAMES_IN_FLIGHT,
        .poolSizeCount = 1,
        .pPoolSizes = &pool_size,
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

        VkDescriptorBufferInfo buffer_info = {
            .buffer = self->shader_storage_buffers[i],
            .offset = 0,
            .range = sizeof(struct shader_storage_object) * self->vertex_count 
        };

        VkWriteDescriptorSet desc_write = {
            .dstSet = self->descriptor_sets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pBufferInfo = &buffer_info,
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET
        };

        vkUpdateDescriptorSets(self->logical_device, 1, &desc_write, 0, NULL);
    }

    free(layouts);
}
