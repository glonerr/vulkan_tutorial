#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "vulkan_utils.h"

#define EXPORTED_VULKAN_FUNCTION(name) extern PFN_##name name;
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#include "vulkan_utils.conf"

int main(void)
{
    LIBRARY_TYPE vulkan_library;
    if (!loadLibrary(vulkan_library))
    {
        exit(EXIT_FAILURE);
    }
    if (!loadExportedVulkanFunction(vulkan_library))
    {
        exit(EXIT_FAILURE);
    }

    if (!loadGlobalLevelFunctions())
    {
        exit(EXIT_FAILURE);
    }

    std::vector<char const *> desired_extensions;
    desired_extensions.emplace_back(VK_KHR_DISPLAY_EXTENSION_NAME);
    VkInstance instance;
    if (!createVulkanInstance(desired_extensions, "my application", instance))
    {
        exit(EXIT_FAILURE);
    }

    if (!loadInstanceLevelFunctions(instance, desired_extensions))
    {
        exit(EXIT_FAILURE);
    }

    std::vector<VkPhysicalDevice> physical_devices;
    if (!enumerateAvailablePhysicalDevices(instance, physical_devices))
    {
        exit(EXIT_FAILURE);
    }

    VkPhysicalDevice physical_device = physical_devices[0];
    std::vector<VkExtensionProperties> available_extensions;
    if (!checkAvailableDeviceExtensions(physical_device, available_extensions))
    {
        exit(EXIT_FAILURE);
    }
    for (auto extension : available_extensions)
    {
        std::cout << extension.extensionName << std::endl;
    }
    VkPhysicalDeviceFeatures device_feature;
    VkPhysicalDeviceProperties device_properties;
    getFeaturesAndPropertiesOfPhysicalDevice(physical_device, device_feature, device_properties);
    puts(device_properties.deviceName);

    std::vector<VkQueueFamilyProperties> device_families;
    checkAvailableQueueFamiliesAndProperties(physical_device, device_families);

    uint32_t queue_family_index;
    if (getIndexOfQueueFamily(physical_device, VK_QUEUE_COMPUTE_BIT, queue_family_index))
    {
        printf("queue_family_index:%d\n", queue_family_index);
    }

    VkPhysicalDeviceFeatures desired_features;
    vkGetPhysicalDeviceFeatures(physical_device, &desired_features);

    std::vector<QueueInfo> queue_infos = {{queue_family_index, {1.0f}}};

    VkDevice logical_device;
    desired_extensions.clear();
    desired_extensions.push_back("VK_EXT_shader_stencil_export");
    if (!createLogicalDevice(physical_device, queue_infos, desired_extensions, &desired_features, logical_device))
    {
        exit(EXIT_FAILURE);
    }
    else
    {
        puts("call createLogicalDevice success!");
    }

    loadDeviceLevelFunctions(logical_device,desired_extensions);
    VkQueue queue;
    getDeviceQueue(logical_device, queue_family_index, 0, queue);
    exit(EXIT_SUCCESS);
}