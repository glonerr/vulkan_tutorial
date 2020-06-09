#include "shader_queue_suit.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
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
void ShaderQueueSuit::destroy_uniform_buffer(VkDevice &device) {}
void ShaderQueueSuit::create_pipeline_layout(VkDevice &device)
{
    VkDescriptorSetLayoutBinding layout_bindings[1] = {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, NULL}}; //这么写可能会出问题
    VkDescriptorSetLayoutCreateInfo layout_ci = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, NULL, 0, 1, layout_bindings};
    descLayouts.resize(NUM_DESCRIPTOR_SETS);
    VkResult result = vkCreateDescriptorSetLayout(device, &layout_ci, NULL, descLayouts.data());
    assert(result == VK_SUCCESS);
    VkPipelineLayoutCreateInfo pipeline_layout_ci = {VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO, NULL, 0, NUM_DESCRIPTOR_SETS, descLayouts.data(), 0, NULL};
    result = vkCreatePipelineLayout(device, &pipeline_layout_ci, NULL, &pipelineLayout);
    assert(result == VK_SUCCESS);
}
void ShaderQueueSuit::destroy_pipeline_layout(VkDevice &device) {}
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
    bool retVal = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, vertStr.c_str(), vtx_spv);
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
    retVal = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, fragStr.c_str(), fraq_spv);
    assert(retVal);
    printf("片元着色器脚本编译SPV成功！\n");
    shader_module_ci.codeSize = fraq_spv.size() * sizeof(unsigned int);
    shader_module_ci.pCode = fraq_spv.data();
    result = vkCreateShaderModule(device, &shader_module_ci, NULL, &shader_stage_cis[1].module);
    assert(result == VK_SUCCESS);

    finalize_glslang();
}
void ShaderQueueSuit::destroy_shader(VkDevice &device) {}
void ShaderQueueSuit::initVertexAttributeInfo() {}
void ShaderQueueSuit::create_pipe_line(VkDevice &device, VkRenderPass &renderPass) {}
void ShaderQueueSuit::destroy_pipe_line(VkDevice &device) {}

ShaderQueueSuit::ShaderQueueSuit(VkDevice *deviceIn, VkRenderPass &renderPass, VkPhysicalDeviceMemoryProperties &memoryroperties)
{
    this->device = deviceIn;
    create_uniform_buffer(*device, memoryroperties);
    create_pipeline_layout(*device);
    init_descriptor_set(*device);
    create_shader(*device);
    initVertexAttributeInfo();
    create_pipe_line(*device, renderPass);
}

ShaderQueueSuit::~ShaderQueueSuit()
{
}