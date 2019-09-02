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

bool loadLibrary(LIBRARY_TYPE &vulkan_library);
bool loadExportedVulkanFunction(LIBRARY_TYPE const &vulkan_library);
bool loadGlobalLevelFunctions();
bool loadInstanceLevelFunctions(VkInstance instance, std::vector<char const *> const &enabled_extensions);
bool checkAvailableInstanceExtensions(std::vector<VkExtensionProperties> &available_extensions);
bool isExtensionSupported(std::vector<VkExtensionProperties> const &available_extensions, char const *const extension);
bool createVulkanInstance(std::vector<char const *> const &desired_extensions, char const *const application_name, VkInstance &instance);
#endif