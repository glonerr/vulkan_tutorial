#include "drawable_object.h"
#include <assert.h>
DrawableObject::DrawableObject(float *vdataIn, int dataByteCount, int vCountIn, VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryroperties)
{
    this->vdata = vdataIn;
    this->device = &device;
    this->vCount = vCountIn;
    VkBufferCreateInfo buffer_ci = {};
    buffer_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_ci.pNext = NULL;
    buffer_ci.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer_ci.size = dataByteCount;
    buffer_ci.queueFamilyIndexCount = 0;
    buffer_ci.pQueueFamilyIndices = NULL;
    buffer_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_ci.flags = 0;
    VkResult result = vkCreateBuffer(device, &buffer_ci, NULL, &vertexDataBuf);
    assert(result == VK_SUCCESS);
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(device, vertexDataBuf, &mem_reqs);
    assert(dataByteCount <= mem_reqs.size);
}
DrawableObject::~DrawableObject() {}
void DrawableObject::drawSelf(VkCommandBuffer &secondary_cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer) {}