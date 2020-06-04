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

// bool checkAvailableDeviceExtensions(VkPhysicalDevice physical_device, std::vector<VkExtensionProperties> &available_extensions)
// {
//     uint32_t extensions_count;
//     VkResult result = vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, nullptr);
//     if ((result != VK_SUCCESS) ||
//         (extensions_count == 0))
//     {
//         std::cout << "Could not get the number of device extensions." << std::endl;
//         return false;
//     }

//     available_extensions.resize(extensions_count);
//     result = vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, available_extensions.data());
//     if ((result != VK_SUCCESS) ||
//         (extensions_count == 0))
//     {
//         std::cout << "Could not enumerate device extensions." << std::endl;
//         return false;
//     }
//     return true;
// }

// bool enumerateAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice> &available_devices)
// {
//     uint32_t devices_count;
//     VkResult result = vkEnumeratePhysicalDevices(instance, &devices_count, nullptr);
//     if ((result != VK_SUCCESS) ||
//         (devices_count == 0))
//     {
//         std::cout << "Could not get the number of available physical devices." << std::endl;
//         return false;
//     }
//     available_devices.resize(devices_count);
//     result = vkEnumeratePhysicalDevices(instance, &devices_count, available_devices.data());
//     if ((result != VK_SUCCESS) ||
//         (devices_count == 0))
//     {
//         std::cout << "Could not enumerate physical devices." << std::endl;
//         return false;
//     }
//     return true;
// }
// void getFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physical_device, VkPhysicalDeviceFeatures &device_features, VkPhysicalDeviceProperties &device_properties)
// {
//     vkGetPhysicalDeviceFeatures(physical_device, &device_features);
//     vkGetPhysicalDeviceProperties(physical_device, &device_properties);
// }
// bool checkAvailableQueueFamiliesAndProperties(VkPhysicalDevice physical_device, std::vector<VkQueueFamilyProperties> &queue_families)
// {
//     uint32_t queue_families_count;
//     vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);
//     if (queue_families_count == 0)
//     {
//         std::cout << "Could not get the number of queue families." << std::endl;
//         return false;
//     }
//     queue_families.resize(queue_families_count);
//     vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, queue_families.data());
//     if (queue_families_count == 0)
//     {
//         std::cout << "Could not acquire properties of queue families." << std::endl;
//         return false;
//     }

//     return true;
// }
// bool getIndexOfQueueFamily(VkPhysicalDevice physical_device, VkQueueFlags desired_capabilities, uint32_t &queue_family_index)
// {
//     std::vector<VkQueueFamilyProperties> queue_families;
//     if (!checkAvailableQueueFamiliesAndProperties(physical_device, queue_families))
//     {
//         return false;
//     }
//     queue_family_index = 0;
//     for (auto family : queue_families)
//     {
//         if ((family.queueCount > 0) &&
//             ((family.queueFlags & desired_capabilities) == desired_capabilities))
//         {
//             return true;
//         }
//         queue_family_index++;
//     }
//     return true;
// }
// bool createLogicalDevice(VkPhysicalDevice physical_device, std::vector<QueueInfo> queue_infos, std::vector<char const *> const &desired_extensions, VkPhysicalDeviceFeatures *desired_features, VkDevice &logical_device)
// {
//     std::vector<VkExtensionProperties> available_extensions;
//     if (!checkAvailableDeviceExtensions(physical_device, available_extensions))
//     {
//         return false;
//     }
//     for (auto extension : desired_extensions)
//     {
//         if (!isExtensionSupported(available_extensions, extension))
//         {
//             std::cout << "Extension named '" << extension << "' is not supported by a physical device." << std::endl;
//             return false;
//         }
//     }

//     std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

//     for (auto &info : queue_infos)
//     {
//         queue_create_infos.push_back({
//             //译者注
//             VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,                //sType
//             nullptr,                                                   //pNext
//             0,                                                         //flags
//             info.FamilyIndex,                                          //queueFamilyIndex
//             static_cast<uint32_t>(info.Priorities.size()),             //queueCount
//             info.Priorities.size() > 0 ? &info.Priorities[0] : nullptr //pQueuePriorities
//         });
//     };

//     VkDeviceCreateInfo device_create_info = {
//         //译者注
//         VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,                             //sType
//         nullptr,                                                          //pNext
//         0,                                                                //flags
//         static_cast<uint32_t>(queue_create_infos.size()),                 //queueCreateInfoCount
//         queue_create_infos.size() > 0 ? &queue_create_infos[0] : nullptr, //pQueueCreateInfos
//         0,                                                                //enabledLayerCount
//         nullptr,                                                          //ppEnabledLayerNames
//         static_cast<uint32_t>(desired_extensions.size()),                 //enabledExtensionCount
//         desired_extensions.size() > 0 ? &desired_extensions[0] : nullptr, //ppEnabledExtensionNames
//         desired_features                                                  //pEnabledFeatures
//     };

//     VkResult result = vkCreateDevice(physical_device, &device_create_info, nullptr, &logical_device);
//     if ((result != VK_SUCCESS) || (logical_device == VK_NULL_HANDLE))
//     {
//         std::cout << "Could not create logical device." << std::endl;
//         return false;
//     }

//     return true;
// }
// bool loadDeviceLevelFunctions(VkDevice logical_device, std::vector<char const *> const &enabled_extensions)
// {
// #define DEVICE_LEVEL_VULKAN_FUNCTION(name)                                                     \
//     name = (PFN_##name)vkGetDeviceProcAddr(logical_device, #name);                             \
//     if (name == nullptr)                                                                       \
//     {                                                                                          \
//         std::cout << "Could not load device-level Vulkan function named: " #name << std::endl; \
//         return false;                                                                          \
//     }

// #include "vulkan_utils.conf"

// #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                                   \
//     for (auto &enabled_extension : enabled_extensions)                                                 \
//     {                                                                                                  \
//         if (std::string(enabled_extension) == std::string(extension))                                  \
//         {                                                                                              \
//             name = (PFN_##name)vkGetDeviceProcAddr(logical_device, #name);                             \
//             if (name == nullptr)                                                                       \
//             {                                                                                          \
//                 std::cout << "Could not load device-level Vulkan function named: " #name << std::endl; \
//                 return false;                                                                          \
//             }                                                                                          \
//         }                                                                                              \
//     }

//     return true;
// }

// void getDeviceQueue(VkDevice logical_device, uint32_t queue_family_index, uint32_t queue_index, VkQueue &queue)
// {
//     vkGetDeviceQueue(logical_device, queue_family_index, queue_index, &queue);
// }