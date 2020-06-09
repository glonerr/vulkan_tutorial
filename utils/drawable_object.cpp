#include "drawable_object.h"
#include "utils.h"
#include "string.h"
#include <stdio.h>
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
    VkMemoryAllocateInfo mem_ai = {};
    mem_ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_ai.pNext = NULL;
    mem_ai.memoryTypeIndex = 0;
    mem_ai.allocationSize = mem_reqs.size;
    VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bool flag = memory_type_from_properties(memoryroperties, mem_reqs.memoryTypeBits, requirements_mask, &mem_ai.memoryTypeIndex);
    if (flag)
    {
        printf("确定内存类型成功，类型索引为%d\n", mem_ai.memoryTypeIndex);
    }
    else
    {
        printf("确定内存类型失败\n");
    }
    result = vkAllocateMemory(device, &mem_ai, NULL, &vertexDataMem);
    assert(result == VK_SUCCESS);
    uint8_t *pData;
    result = vkMapMemory(device, vertexDataMem, 0, mem_reqs.size, 0, (void **)&pData);
    assert(result == VK_SUCCESS);
    memcpy(pData, vdata, dataByteCount);
    vkUnmapMemory(device, vertexDataMem);
    result = vkBindBufferMemory(device, vertexDataBuf, vertexDataMem, 0);
    assert(result == VK_SUCCESS);
    vertexDataBufferInfo.buffer = vertexDataBuf;
    vertexDataBufferInfo.offset = 0;
    vertexDataBufferInfo.range = mem_reqs.size;
}
DrawableObject::~DrawableObject()
{
    delete vdata;
    vkDestroyBuffer(*device, vertexDataBuf, NULL);
    vkFreeMemory(*device, vertexDataMem, NULL);
}
void DrawableObject::drawSelf(VkCommandBuffer &secondary_cmd, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline, VkDescriptorSet *desSetPointer)
{
    vkCmdBindPipeline(secondary_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindDescriptorSets(secondary_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, desSetPointer, 0, NULL);
    VkDeviceSize offsetsVertex[1] = {0};
    vkCmdBindVertexBuffers(secondary_cmd, 0, 1, &vertexDataBuf, offsetsVertex);
    vkCmdDraw(secondary_cmd, vCount, 1, 0, 0);
}