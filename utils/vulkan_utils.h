#ifndef VULKAN_UTIL
#define VULKAN_UTIL
#define VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#define LIBRARY_TYPE HMODULE
#elif defined __linux
#include <dlfcn.h>
#define LIBRARY_TYPE void *
#endif

struct QueueInfo
{
    uint32_t FamilyIndex;
    std::vector<float> Priorities;
};

bool loadLibrary(LIBRARY_TYPE &vulkan_library);
bool loadExportedVulkanFunction(LIBRARY_TYPE const &vulkan_library); //
bool loadGlobalLevelFunctions();                                     //
bool loadInstanceLevelFunctions(VkInstance instance, std::vector<char const *> const &enabled_extensions);
bool loadDeviceLevelFunctions(VkDevice logical_device, std::vector<char const *> const &enabled_extensions);
bool checkAvailableInstanceExtensions(std::vector<VkExtensionProperties> &available_extensions);
bool checkAvailableDeviceExtensions(VkPhysicalDevice physical_device, std::vector<VkExtensionProperties> &available_extensions);
bool checkAvailableQueueFamiliesAndProperties(VkPhysicalDevice physical_device, std::vector<VkQueueFamilyProperties> &queue_families);
bool createVulkanInstance(std::vector<char const *> const &desired_extensions, char const *const application_name, VkInstance &instance);
bool createLogicalDevice(VkPhysicalDevice physical_device, std::vector<QueueInfo> queue_infos, std::vector<char const *> const &desired_extensions, VkPhysicalDeviceFeatures *desired_features, VkDevice &logical_device);
bool createPresentationSurface(VkInstance instance);
bool isExtensionSupported(std::vector<VkExtensionProperties> const &available_extensions, char const *const extension);
bool enumerateAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice> &available_devices);
void getDeviceQueue(VkDevice logical_device,uint32_t queue_family_index,uint32_t queue_index,VkQueue &queue);
void getFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physical_device, VkPhysicalDeviceFeatures &device_features, VkPhysicalDeviceProperties &device_properties);
bool getIndexOfQueueFamily(VkPhysicalDevice physical_device, VkQueueFlags desired_capabilities, uint32_t &queue_family_index);
#endif