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
    
    VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state = {
        .dynamicStateCount = sizeof(dynamic_states) / sizeof(VkDynamicState), 
        .pDynamicStates = dynamic_states, 
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO
    };
    VkPipelineVertexInputStateCreateInfo vertex_input_info =  {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
    };
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    VkPipelineViewportStateCreateInfo  viewport_state = {.viewportCount = 1, .scissorCount = 1, .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0f,
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
    };
    VkPipelineMultisampleStateCreateInfo multisampling = {
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO
    };
    VkPipelineColorBlendAttachmentState color_blend_attachment = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask =  VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };
    VkPipelineColorBlendStateCreateInfo color_blending = {
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY, 
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment,
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO
    };
    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .setLayoutCount = 0, 
        .pushConstantRangeCount = 0,
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    };
    vkCreatePipelineLayout(self->logical_device, &pipeline_layout_info, NULL, &self->pipeline_layout);

    VkPipelineRenderingCreateInfo pipeline_rendering_create_info = {
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &self->swap_chain_surface_format.format,
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = NULL
    };
    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info_chain[] = {
        {
            .stageCount = 2,
            .pStages = shader_stages,
            .pVertexInputState = &vertex_input_info,
            .pInputAssemblyState = &input_assembly,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pColorBlendState = &color_blending,
            .pDynamicState = &dynamic_state,
            .layout = self->pipeline_layout,
            .renderPass = NULL,
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &pipeline_rendering_create_info
        }
    };

    vkCreateGraphicsPipelines(self->logical_device, NULL, 1, graphics_pipeline_create_info_chain, NULL, &self->graphics_pipeline);

    vkDestroyShaderModule(self->logical_device, module, NULL);
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