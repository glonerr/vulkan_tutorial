#ifndef DRAWABLE_OBJECT_H
#define DRAWABLE_OBJECT_H
#ifdef USE_VULKAN_WRAPPER
#include "vulkan_wrapper.h"
#else
#include <vulkan/vulkan.h>
#endif

class DrawableObject
{
    VkDevice *device;
    float *vdata;
    int vCount;
    VkBuffer vertexDataBuf;
    VkDeviceMemory vertexDataMem;
    VkDescriptorBufferInfo vertexDataBufferInfo;

public:
    DrawableObject(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryroperties);
    ~DrawableObject();
    void drawSelf(VkCommandBuffer &secondary_cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer);
};
#endif