#include "vulkan_wrapper.h"
#include <vector>
bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader,
               std::vector<unsigned int> &spirv);
void init_glslang();
void finalize_glslang();
/* 管线布置创作，创作和描述符设置布局数 */
#define NUM_DESCRIPTOR_SETS 1