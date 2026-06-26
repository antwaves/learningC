#include "app.h"
#include "vulkan/vulkan_core.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <corecrt.h>

static char* read_shader_binary(const char* filename, uint32_t *b_size);
[[nodiscard]] VkShaderModule create_shader_module(VkDevice device, const char* bytes, uint32_t size);

void _create_graphics_pipeline(struct App* self) {
    uint32_t b_size = 0;
    char* shader_bytes = read_shader_binary("shaders\\bin\\triangle.spv", &b_size);
    VkShaderModule module = create_shader_module(self->logical_device, shader_bytes, b_size);

    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, 
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = module,
        .pName = "vert_main"
    };
    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, 
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = module,
        .pName = "frag_main"
    };
    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

    
    free(shader_bytes);
}

static char* read_shader_binary(const char* filename, uint32_t *b_size) {
    FILE* fp;
    errno_t result = fopen_s(&fp, filename, "rb");

    if (fp == NULL) {
        printf("Failed to open file with error code %d", result);
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0L, SEEK_END);
    uint32_t size = ftell(fp);
    *b_size = size;
    fseek(fp, 0L, SEEK_SET);
    char* buffer = calloc(size, sizeof(char));
    fread_s(buffer, size * sizeof(char), sizeof(char), size, fp);
    fclose(fp);
    return buffer;
}

[[nodiscard]] VkShaderModule create_shader_module(VkDevice device, const char* bytes, uint32_t size) {
    VkShaderModule shader_module;
    VkShaderModuleCreateInfo shader_module_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, 
        .codeSize = size,
        .pCode = (uint32_t*)(bytes)
    };
    vkCreateShaderModule(device, &shader_module_create_info, NULL, &shader_module);
    return shader_module;
}