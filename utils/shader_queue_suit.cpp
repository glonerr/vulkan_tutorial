#include "shader_queue_suit.h"
#include <assert.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <iostream>

void ShaderQueueSuit::create_uniform_buffer(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryproperties)
{
    bufferByteCount = sizeof(float) * 16;
    VkBufferCreateInfo buffer_ci = {};
    buffer_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_ci.pNext = NULL;
    buffer_ci.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buffer_ci.size = bufferByteCount;
    buffer_ci.queueFamilyIndexCount = 0;
    buffer_ci.pQueueFamilyIndices = NULL;
    buffer_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_ci.flags = 0;
    VkResult result = vkCreateBuffer(device, &buffer_ci, NULL, &uniformBuf);
    assert(result == VK_SUCCESS);
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(device, uniformBuf, &mem_reqs);
    VkMemoryAllocateInfo mem_ai = {};
    mem_ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_ai.pNext = NULL;
    mem_ai.memoryTypeIndex = 0;
    mem_ai.allocationSize = mem_reqs.size;
    VkFlags requirments_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bool flag = memory_type_from_properties(memoryproperties, mem_reqs.memoryTypeBits, requirments_mask, &mem_ai.memoryTypeIndex);
    if (flag)
    {
        printf("确定内存类型成功，类型索引为%d\n", mem_ai.memoryTypeIndex);
    }
    else
    {
        printf("确定内存类型失败\n");
    }
    result = vkAllocateMemory(device, &mem_ai, NULL, &memUniformBuf);
    assert(result == VK_SUCCESS);
    result = vkBindBufferMemory(device, uniformBuf, memUniformBuf, 0);
    assert(result == VK_SUCCESS);

    uniformBufferInfo.buffer = uniformBuf;
    uniformBufferInfo.offset = 0;
    uniformBufferInfo.range = bufferByteCount;
}

void ShaderQueueSuit::destroy_uniform_buffer(VkDevice &device)
{
    vkDestroyBuffer(device, uniformBuf, NULL);
    vkFreeMemory(device, memUniformBuf, NULL);
}

void ShaderQueueSuit::create_pipeline_layout(VkDevice &device)
{
    VkDescriptorSetLayoutBinding layout_bindings[1] = {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, NULL}}; //这么写可能会出问题
    VkDescriptorSetLayoutCreateInfo layout_ci = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, NULL, 0, 1, layout_bindings};

    descLayouts.resize(NUM_DESCRIPTOR_SETS);
    VkResult result = vkCreateDescriptorSetLayout(device, &layout_ci, NULL, descLayouts.data());
    assert(result == VK_SUCCESS);

    VkPipelineLayoutCreateInfo pipeline_layout_ci = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, NULL, 0, NUM_DESCRIPTOR_SETS, descLayouts.data(), 0, NULL};
    result = vkCreatePipelineLayout(device, &pipeline_layout_ci, NULL, &pipelineLayout);
    assert(result == VK_SUCCESS);
}

void ShaderQueueSuit::destroy_pipeline_layout(VkDevice &device)
{
    for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++)
    {
        vkDestroyDescriptorSetLayout(device, descLayouts[i], NULL);
    }
    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
}

void ShaderQueueSuit::init_descriptor_set(VkDevice &device)
{
    VkDescriptorPoolSize type_count[1] = {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}};
    VkDescriptorPoolCreateInfo desc_pool_ci = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, NULL, 0, 1, 1, type_count};
    VkResult result = vkCreateDescriptorPool(device, &desc_pool_ci, NULL, &descPool);
    assert(result == VK_SUCCESS);
    std::vector<VkDescriptorSetLayout> layouts;
    layouts.push_back(descLayouts[0]);
    VkDescriptorSetAllocateInfo desc_set_ai[1] = {{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, NULL, descPool, 1, layouts.data()}};
    descSet.resize(1);
    result = vkAllocateDescriptorSets(device, desc_set_ai, descSet.data());
    assert(result == VK_SUCCESS);
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = NULL;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &uniformBufferInfo;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;
}
void ShaderQueueSuit::create_shader(VkDevice &device)
{
    printf("create_shader\n");
    init_glslang();

    std::ifstream t("shaders/vertshadertext.vert");
    std::stringstream ver_buffer;
    ver_buffer << t.rdbuf();
    std::string vertStr = ver_buffer.str();

    std::ifstream f("shaders/fragshadertext.frag");
    std::stringstream fraq_buffer;
    fraq_buffer << f.rdbuf();
    std::string fragStr = fraq_buffer.str();

    shader_stage_cis[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_cis[0].pNext = NULL;
    shader_stage_cis[0].pSpecializationInfo = NULL;
    shader_stage_cis[0].flags = 0;
    shader_stage_cis[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stage_cis[0].pName = "main";

    std::vector<unsigned int> vtx_spv;
    const char *vertShaderText =
        "#version 400\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#extension GL_ARB_shading_language_420pack : enable\n"
        "layout (std140, binding = 0) uniform bufferVals {\n"
        "    mat4 mvp;\n"
        "} myBufferVals;\n"
        "layout (location = 0) in vec4 pos;\n"
        "layout (location = 1) in vec4 inColor;\n"
        "layout (location = 0) out vec4 outColor;\n"
        "void main() {\n"
        "   outColor = inColor;\n"
        "   gl_Position = myBufferVals.mvp * pos;\n"
        "}\n";

    const char *fragShaderText =
        "#version 400\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#extension GL_ARB_shading_language_420pack : enable\n"
        "layout (location = 0) in vec4 color;\n"
        "layout (location = 0) out vec4 outColor;\n"
        "void main() {\n"
        "   outColor = color;\n"
        "}\n";
    bool retVal = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT,vertShaderText/* vertStr.c_str() */, vtx_spv);
    assert(retVal);
    printf("顶点着色器脚本编译SPV成功！\n");
    VkShaderModuleCreateInfo shader_module_ci = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, NULL, 0, vtx_spv.size() * sizeof(unsigned int), vtx_spv.data()};
    VkResult result = vkCreateShaderModule(device, &shader_module_ci, NULL, &shader_stage_cis[0].module);
    assert(result == VK_SUCCESS);

    shader_stage_cis[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_cis[1].pNext = NULL;
    shader_stage_cis[1].flags = 0;
    shader_stage_cis[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stage_cis[1].pName = "main";

    std::vector<unsigned int> fraq_spv;
    retVal = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderText/* fragStr.c_str() */, fraq_spv);
    assert(retVal);
    printf("片元着色器脚本编译SPV成功！\n");
    shader_module_ci.codeSize = fraq_spv.size() * sizeof(unsigned int);
    shader_module_ci.pCode = fraq_spv.data();
    result = vkCreateShaderModule(device, &shader_module_ci, NULL, &shader_stage_cis[1].module);
    assert(result == VK_SUCCESS);

    finalize_glslang();
}

void ShaderQueueSuit::destroy_shader(VkDevice &device)
{
    vkDestroyShaderModule(device, shader_stage_cis[0].module, NULL);
    vkDestroyShaderModule(device, shader_stage_cis[1].module, NULL);
}

void ShaderQueueSuit::initVertexAttributeInfo()
{
    vertexBinding.binding = 0;
    vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexBinding.stride = sizeof(float) * 6;
    vertexAttribs[0].binding = 0;
    vertexAttribs[0].location = 0;
    vertexAttribs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexAttribs[0].offset = 0;
    vertexAttribs[1].binding = 0;
    vertexAttribs[1].location = 1;
    vertexAttribs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexAttribs[1].offset = 12;
}

void ShaderQueueSuit::create_pipe_line(VkDevice &device, VkRenderPass &renderPass)
{
    VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
    memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);

    VkPipelineDynamicStateCreateInfo dynamic_state_ci = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, NULL, 0, 0, dynamicStateEnables};

    VkPipelineVertexInputStateCreateInfo vertex_input_state_ci = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, NULL, 0, 1, &vertexBinding, 2, vertexAttribs};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_ci = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, NULL, 0, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE};

    VkPipelineRasterizationStateCreateInfo rasterization_state_ci = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, NULL, 0, VK_TRUE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0, 0, 0, 1.0f};

    VkPipelineColorBlendAttachmentState att_state[1] = {{VK_FALSE, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, 0xf}};

    VkPipelineColorBlendStateCreateInfo color_blend_state_ci = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, NULL, 0, VK_FALSE, VK_LOGIC_OP_NO_OP, 1, att_state, {1.0f, 1.0f, 1.0f, 1.0f}};

    VkViewport viewports = {0, 0, (float)info->width, (float)info->height, 0.0f, 1.0f};
    VkRect2D scissor = {{0, 0}, {info->width, info->height}};

    VkPipelineViewportStateCreateInfo viewport_state_ci = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, NULL, 0, 1, &viewports, 1, &scissor};

    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_ci = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, NULL, 0, VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL, VK_FALSE, VK_FALSE, {VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS, 0, 0, 0}, {VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS, 0, 0, 0}};

    VkPipelineMultisampleStateCreateInfo multi_sample_state_ci = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, NULL, 0, VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0, NULL, VK_FALSE, VK_FALSE};

    VkGraphicsPipelineCreateInfo graphics_pipeline_ci = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, NULL, 0, 2, shader_stage_cis, &vertex_input_state_ci, &input_assembly_state_ci, NULL, &viewport_state_ci, &rasterization_state_ci, &multi_sample_state_ci, &depth_stencil_state_ci, &color_blend_state_ci, &dynamic_state_ci, pipelineLayout, renderPass, 0, VK_NULL_HANDLE, 0};

    VkPipelineCacheCreateInfo pipeline_cache_ci = {VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO, NULL, 0, 0, NULL};

    VkResult result = vkCreatePipelineCache(device, &pipeline_cache_ci, NULL, &pipelineCache);

    assert(result == VK_SUCCESS);

    result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &graphics_pipeline_ci, NULL, NULL);

    assert(result == VK_SUCCESS);
}

void ShaderQueueSuit::destroy_pipe_line(VkDevice &device)
{
    vkDestroyPipeline(device, pipeline, NULL);
    vkDestroyPipelineCache(device, pipelineCache, NULL);
}

ShaderQueueSuit::ShaderQueueSuit(VkRenderPass &renderPass, VkPhysicalDeviceMemoryProperties &memoryroperties, struct window_info &info)
{
    this->info = &info;
    create_uniform_buffer(info.device, memoryroperties);
    create_pipeline_layout(info.device);
    init_descriptor_set(info.device);
    create_shader(info.device);
    initVertexAttributeInfo();
    create_pipe_line(info.device, renderPass);
}

ShaderQueueSuit::~ShaderQueueSuit()
{
    destroy_pipe_line(info->device);       //销毁管线
    destroy_shader(info->device);          //销毁着色器模块
    destroy_pipeline_layout(info->device); //销毁管线布局
    destroy_uniform_buffer(info->device);  //销毁一致变量缓冲
}