#include <iostream>
#include <cstring>
#include "vulkan_utils.h"

#define EXPORTED_VULKAN_FUNCTION(name) PFN_##name name;
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name;
#include "vulkan_utils.conf"

bool loadLibrary(LIBRARY_TYPE &vulkan_library)
{
#if defined _WIN32
    vulkan_library = LoadLibrary("vulkan-1.dll");
#elif defined __linux
    vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW);
#endif
    if (vulkan_library == nullptr)
    {
        std::cout << "Could not load Vulkan Runtime library." << std::endl;
        return false;
    }
    return true;
}

bool loadExportedVulkanFunction(LIBRARY_TYPE const &vulkan_library)
{
#if defined _WIN32
#define LoadFunction GetProcAddress
#elif defined __linux
#define LoadFunction dlsym
#endif
#define EXPORTED_VULKAN_FUNCTION(name)                                                     \
    vkGetInstanceProcAddr = (PFN_##name)LoadFunction(vulkan_library, #name);               \
    if (name == nullptr)                                                                   \
    {                                                                                      \
        std::cout << "Could not load exported Vulkan function named: " #name << std::endl; \
        return false;                                                                      \
    }

#include "vulkan_utils.conf"

    return true;
}

bool loadGlobalLevelFunctions()
{
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                                     \
    name = (PFN_##name)vkGetInstanceProcAddr(nullptr, #name);                                  \
    if (name == nullptr)                                                                       \
    {                                                                                          \
        std::cout << "Could not load global level Vulkan function named: " #name << std::endl; \
        return false;                                                                          \
    }

#include "vulkan_utils.conf"

    return true;
}

bool checkAvailableInstanceExtensions(std::vector<VkExtensionProperties> &available_extensions)
{
    uint32_t extensions_count = 0;
    VkResult result = VK_SUCCESS;

    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
    if ((result != VK_SUCCESS) ||
        (extensions_count == 0))
    {
        std::cout << "Could not get the number of instance extensions." << std::endl;
        return false;
    }

    available_extensions.resize(extensions_count);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data());
    if ((result != VK_SUCCESS) ||
        (extensions_count == 0))
    {
        std::cout << "Could not enumerate instance extensions." << std::endl;
        return false;
    }

    return true;
}

bool isExtensionSupported(std::vector<VkExtensionProperties> const &available_extensions, char const *const extension)
{
    for (auto &available_extension : available_extensions)
    {
        if (strstr(available_extension.extensionName, extension))
        {
            return true;
        }
    }
    return false;
}

bool createVulkanInstance(std::vector<char const *> const &desired_extensions,
                          char const *const application_name,
                          VkInstance &instance)
{
    std::vector<VkExtensionProperties> available_extensions;
    if (!checkAvailableInstanceExtensions(available_extensions))
    {
        return false;
    }

    for (auto &extension : desired_extensions)
    {
        if (!isExtensionSupported(available_extensions, extension))
        {
            std::cout << "Extension named '" << extension << "' is not supported by an Instance object." << std::endl;
            return false;
        }
    }

    VkApplicationInfo application_info = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO, // VkStructureType           sType
        nullptr,                            // const void              * pNext
        application_name,                   // const char              * pApplicationName
        VK_MAKE_VERSION(1, 0, 0),           // uint32_t                  applicationVersion
        "Vulkan Cookbook",                  // const char              * pEngineName
        VK_MAKE_VERSION(1, 0, 0),           // uint32_t                  engineVersion
        VK_MAKE_VERSION(1, 0, 0)            // uint32_t                  apiVersion
    };

    VkInstanceCreateInfo instance_create_info = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,           // VkStructureType           sType
        nullptr,                                          // const void              * pNext
        0,                                                // VkInstanceCreateFlags     flags
        &application_info,                                // const VkApplicationInfo * pApplicationInfo
        0,                                                // uint32_t                  enabledLayerCount
        nullptr,                                          // const char * const      * ppEnabledLayerNames
        static_cast<uint32_t>(desired_extensions.size()), // uint32_t                  enabledExtensionCount
        desired_extensions.data()                         // const char * const      * ppEnabledExtensionNames
    };

    VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance);
    if ((result != VK_SUCCESS) ||
        (instance == VK_NULL_HANDLE))
    {
        std::cout << "Could not create Vulkan instance." << std::endl;
        return false;
    }

    return true;
}
bool loadInstanceLevelFunctions(VkInstance instance, std::vector<char const *> const &enabled_extensions)
{
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                                     \
    name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                                   \
    if (name == nullptr)                                                                         \
    {                                                                                            \
        std::cout << "Could not load instance-level Vulkan function named: " #name << std::endl; \
        return false;                                                                            \
    }

    // Load instance-level functions from enabled extensions
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                                   \
    for (auto &enabled_extension : enabled_extensions)                                                   \
    {                                                                                                    \
        if (std::string(enabled_extension) == std::string(extension))                                    \
        {                                                                                                \
            name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                                   \
            if (name == nullptr)                                                                         \
            {                                                                                            \
                std::cout << "Could not load instance-level Vulkan function named: " #name << std::endl; \
                return false;                                                                            \
            }                                                                                            \
        }                                                                                                \
    }

#include "vulkan_utils.conf"

    return true;
}