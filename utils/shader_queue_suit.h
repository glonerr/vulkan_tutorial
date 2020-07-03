#ifndef SHADER_QUEUE_SUIT_H
#define SHADER_QUEUE_SUIT_H
#ifdef USE_VULKAN_WRAPPER
#include "vulkan_wrapper.h"
#else
#include <vulkan/vulkan.h>
#endif
#include "utils.h"
#include <vector>
class ShaderQueueSuit
{
    VkBuffer uniformBuf;
    VkDescriptorBufferInfo uniformBufferInfo;
    std::vector<VkDescriptorSetLayout> descLayouts;
    VkPipelineShaderStageCreateInfo shader_stage_cis[2];
    VkVertexInputBindingDescription vertexBinding;
    VkVertexInputAttributeDescription vertexAttribs[2];
    VkPipelineCache pipelineCache;
    VkDescriptorPool descPool;
    window_info *info;

    void create_uniform_buffer(VkDevice &device,VkPhysicalDeviceMemoryProperties &memoryproperties);
    void destroy_uniform_buffer(VkDevice &device);
    void create_pipeline_layout(VkDevice &device);
    void destroy_pipeline_layout(VkDevice &device);
    void init_descriptor_set(VkDevice &device);
    void create_shader(VkDevice &device);
    void destroy_shader(VkDevice &device);
    void initVertexAttributeInfo();
    void create_pipe_line(VkDevice &device,VkRenderPass &renderPass);
    void destroy_pipe_line(VkDevice &device);

public:
    int bufferByteCount;
    VkDeviceMemory memUniformBuf;
    VkWriteDescriptorSet writes[1];
    std::vector<VkDescriptorSet> descSet;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    ShaderQueueSuit(VkRenderPass &renderPass, VkPhysicalDeviceMemoryProperties &memoryroperties,struct window_info &info);
    ~ShaderQueueSuit();
};
#endif