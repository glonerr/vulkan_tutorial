#ifndef DRAWABLE_OBJECT_H
#define DRAWABLE_OBJECT_H
#include "vulkan_wrapper.h"
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