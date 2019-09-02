#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "vulkan_utils.h"

#define EXPORTED_VULKAN_FUNCTION(name) extern PFN_##name name;
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
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
    VkInstance instance;
    if (!createVulkanInstance(desired_extensions, "my application", instance))
    {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}