#include "vulkan_manager.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
void init_vulkan_instance()
{
    instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    instanceExtensionNames.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = "HelloVulkan";
    app_info.applicationVersion = 1;
    app_info.pEngineName = "HelloVulkan";
    app_info.engineVersion = 1;
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledExtensionCount = instanceExtensionNames.size();
    inst_info.ppEnabledExtensionNames = instanceExtensionNames.data();
    inst_info.enabledLayerCount = 0;
    inst_info.ppEnabledLayerNames = NULL;
    VkResult result;
    result = vkCreateInstance(&inst_info, NULL, &instance);
    if (result == VK_SUCCESS)
    {
        printf("Vulkan实例创建成功！\n");
    }
    else
    {
        printf("Vulkan实例创建失败！\n");
    }
}

void enumerate_vulkan_phy_device()
{
    gpuCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
    assert(result == VK_SUCCESS);
    printf("Vulkan硬件设备数量为%d个\n", gpuCount);
    gpus.resize(gpuCount);
    result = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());
    assert(result == VK_SUCCESS);
    vkGetPhysicalDeviceMemoryProperties(gpus[0], &memoryproperties);
}

void create_vulkan_device()
{
    vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueFamilyCount, NULL);
    printf("Vulkan硬件设备0支持的队列家族数量%d个\n", queueFamilyCount);
    queueFamilyProps.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueFamilyCount, queueFamilyProps.data());
    printf("成功获取Vulkan硬件设备0支持的队列家族属性列表\n");
    VkDeviceQueueCreateInfo queueInfo = {};
    bool found = false;
    for (unsigned int i = 0; i < queueFamilyCount; i++)
    {
        printf("队列家族属性标志位0x%08x\n", queueFamilyProps[i].queueFlags);
        if (queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueInfo.queueFamilyIndex = i;
            queueGraphicsFamilyIndex = i;
            printf("支持GRAPHIC工作的一个队列家族索引为%d\n", i);
            printf("此家族中的实际队列数量是%d\n", queueFamilyProps[i].queueCount);
            found = true;
            break;
        }
    }
    float queue_priorities[1] = {0.0};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = NULL;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queue_priorities;
    // queueInfo.queueFamilyIndex =
    deviceExtensionName.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = NULL;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = 1;
    deviceInfo.ppEnabledExtensionNames = deviceExtensionName.data();
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = NULL;
    deviceInfo.pEnabledFeatures = NULL;
    VkResult result = vkCreateDevice(gpus[0], &deviceInfo, NULL, &device);
    assert(result == VK_SUCCESS);
}

void create_vulkan_CommandBuffer()
{
    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = NULL;
    cmd_pool_info.queueFamilyIndex = queueGraphicsFamilyIndex;
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkResult result = vkCreateCommandPool(device, &cmd_pool_info, NULL, &cmdPool);
    assert(result == VK_SUCCESS);
    VkCommandBufferAllocateInfo cmdBAI = {};
    cmdBAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBAI.pNext = NULL;
    cmdBAI.commandPool = cmdPool;
    cmdBAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBAI.commandBufferCount = 1;
    result = vkAllocateCommandBuffers(device, &cmdBAI, &cmdBuffer);
    assert(result == VK_SUCCESS);
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buf_info.pNext = NULL;
    cmd_buf_info.flags = 0;
    cmd_buf_info.pInheritanceInfo = NULL;
    cmd_bufs[0] = cmdBuffer;

    VkPipelineStageFlags *pipe_stage_flags = new VkPipelineStageFlags();
    *pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submit_info[0].pWaitDstStageMask = pipe_stage_flags;
    submit_info[0].commandBufferCount = 1;
    submit_info[0].pCommandBuffers = cmd_bufs;
    submit_info[0].signalSemaphoreCount = 0;
    submit_info[0].pSignalSemaphores = NULL;
    printf("创建命令缓冲\n");
}

void init_queue()
{
    vkGetDeviceQueue(device, queueGraphicsFamilyIndex, 0, &queueGraphics);
}

void create_vulkan_swapchain(struct window_info &info)
{
    VkResult result;
    // Construct the surface description:
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.hinstance = info.connection;
    createInfo.hwnd = info.window;
    result = vkCreateWin32SurfaceKHR(info.inst, &createInfo, NULL, &info.surface);
#elif defined(__ANDROID__)
    GET_INSTANCE_PROC_ADDR(info.inst, CreateAndroidSurfaceKHR);

    VkAndroidSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.window = AndroidGetApplicationWindow();
    result = info.fpCreateAndroidSurfaceKHR(info.inst, &createInfo, nullptr, &info.surface);
#elif defined(VK_USE_PLATFORM_METAL_EXT)
    VkMetalSurfaceCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pLayer = info.caMetalLayer;
    result = vkCreateMetalSurfaceEXT(info.inst, &createInfo, NULL, &info.surface);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    VkWaylandSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.display = info.display;
    createInfo.surface = info.window;
    result = vkCreateWaylandSurfaceKHR(info.inst, &createInfo, NULL, &info.surface);
#else
    VkXcbSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.connection = info.connection;
    createInfo.window = info.window;
    result = vkCreateXcbSurfaceKHR(instance, &createInfo, NULL, &surface);
#endif // __ANDROID__  && _WIN32
    VkBool32 *pSupportsPresent = (VkBool32 *)malloc(queueFamilyCount * sizeof(VkBool32));
    for (size_t i = 0; i < queueFamilyCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(gpus[0], i, surface, &pSupportsPresent[i]);
        printf("队列家族索引=%ld %s显示\n", i, pSupportsPresent[i] == 1 ? "支持" : "不支持");
    }
    queueGraphicsFamilyIndex = UINT32_MAX;
    queuePresentFamilyIndex = UINT32_MAX;
    for (size_t i = 0; i < queueFamilyCount; i++)
    {
        if ((queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            if (queueGraphicsFamilyIndex == UINT32_MAX)
            {
                queueGraphicsFamilyIndex = i;
            }
            if (pSupportsPresent[i] == VK_TRUE)
            {
                queueGraphicsFamilyIndex = i;
                queuePresentFamilyIndex = i;
                printf("队列家族索引=%ld同时支持Graphcs(图形)和Present(呈现)工作\n", i);
                break;
            }
        }
    }
    if (queuePresentFamilyIndex == UINT32_MAX)
    {
        for (size_t i = 0; i < queueFamilyCount; i++)
        {
            if (pSupportsPresent[i] == VK_TRUE)
            {
                queuePresentFamilyIndex = i;
                break;
            }
        }
    }
    free(pSupportsPresent);
    if (queueGraphicsFamilyIndex == UINT32_MAX || queuePresentFamilyIndex == UINT32_MAX)
    {
        printf("没有找到支持Graphics(图形)或者Present(呈现或显示)工作的队列家族\n");
        assert(false);
    }

    uint32_t formatCount;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, NULL);
    printf("物理设备表面格式种类:%d\n", formatCount);
    formats.resize(formatCount);
    VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, surfFormats);
    for (size_t i = 0; i < formatCount; i++)
    {
        formats[i] = surfFormats[i].format;
        printf("支持的格式为:%d\n", formats[i]);
    }
    if (formatCount == 1 && formats[0] == VK_FORMAT_UNDEFINED)
    {
        formats[0] = VK_FORMAT_B8G8R8A8_UNORM;
    }
    free(surfFormats);
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpus[0], surface, &surfCapabilities);
    printf("最小图像数:%d 最大图像数:%d\n", surfCapabilities.minImageCount, surfCapabilities.maxImageCount);
    assert(result == VK_SUCCESS);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount, NULL);
    printf("显示方式数量:%d\n", presentModeCount);
    presentModes.resize(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount, presentModes.data());
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < presentModeCount; i++)
    {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }
    if (surfCapabilities.currentExtent.width == 0xFFFFFFFF)
    {
        swapchainExtent.width = screenWidth;
        swapchainExtent.height = screenHeight;
        if (swapchainExtent.width < surfCapabilities.minImageExtent.width)
        {
            swapchainExtent.width = surfCapabilities.minImageExtent.width;
        }
        else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width)
        {
            swapchainExtent.width = surfCapabilities.maxImageExtent.width;
        }

        if (swapchainExtent.height < surfCapabilities.minImageExtent.height)
        {
            swapchainExtent.height = surfCapabilities.minImageExtent.height;
        }
        else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height)
        {
            swapchainExtent.height = surfCapabilities.maxImageExtent.height;
        }
        printf("使用自己设置的宽度 %d 高度 %d\n", swapchainExtent.width, swapchainExtent.height);
    }
    else
    {
        swapchainExtent = surfCapabilities.currentExtent;
        printf("使用获取能力中的宽度 %d 高度 %d\n", swapchainExtent.width, swapchainExtent.height);
    }
    screenWidth = swapchainExtent.width;
    screenHeight = swapchainExtent.height;
    uint32_t desiredMinNumberOfSwachainImages = surfCapabilities.minImageCount + 1;
    if ((surfCapabilities.maxImageCount > 0) && desiredMinNumberOfSwachainImages > surfCapabilities.maxImageCount)
    {
        desiredMinNumberOfSwachainImages = surfCapabilities.maxImageCount;
    }
    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        preTransform = surfCapabilities.currentTransform;
    }
    VkSwapchainCreateInfoKHR swapchain_ci = {};
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.pNext = NULL;
    swapchain_ci.surface = surface;
    swapchain_ci.minImageCount = desiredMinNumberOfSwachainImages;
    swapchain_ci.imageFormat = formats[0];
    swapchain_ci.imageExtent.width = swapchainExtent.width;
    swapchain_ci.imageExtent.height = swapchainExtent.height;
    swapchain_ci.preTransform = preTransform;
    swapchain_ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.presentMode = swapchainPresentMode;
    swapchain_ci.oldSwapchain = VK_NULL_HANDLE;
    swapchain_ci.clipped = true;
    swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_ci.queueFamilyIndexCount = 0;
    swapchain_ci.pQueueFamilyIndices = NULL;
    if (queueGraphicsFamilyIndex != queuePresentFamilyIndex)
    {
        swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_ci.queueFamilyIndexCount = 2;
        uint32_t queueFamilyIndics[2] = {queueGraphicsFamilyIndex, queuePresentFamilyIndex};
        swapchain_ci.pQueueFamilyIndices = queueFamilyIndics;
    }
    result = vkCreateSwapchainKHR(device, &swapchain_ci, NULL, &swapChain);
    assert(result == VK_SUCCESS);
    result = vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, NULL);
    assert(result == VK_SUCCESS);
    printf("交换链图像数量:%d\n", swapchainImageCount);
    swapchainImages.resize(swapchainImageCount);
    result = vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, swapchainImages.data());
    printf("交换链图像单张大小:%ld\n", sizeof(swapchainImages[0]));
    assert(result == VK_SUCCESS);
    swapchainImageViews.resize(swapchainImageCount);
    for (size_t i = 0; i < swapchainImageCount; i++)
    {
        VkImageViewCreateInfo image_view_ci = {};
        image_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_ci.pNext = NULL;
        image_view_ci.flags = 0;
        image_view_ci.image = swapchainImages[i];
        image_view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_ci.format = formats[0];
        image_view_ci.components.r = VK_COMPONENT_SWIZZLE_R;
        image_view_ci.components.g = VK_COMPONENT_SWIZZLE_G;
        image_view_ci.components.b = VK_COMPONENT_SWIZZLE_B;
        image_view_ci.components.a = VK_COMPONENT_SWIZZLE_A;
        image_view_ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_ci.subresourceRange.baseMipLevel = 0;
        image_view_ci.subresourceRange.layerCount = 1;
        image_view_ci.subresourceRange.baseArrayLayer = 0;
        image_view_ci.subresourceRange.layerCount = 1;
        result = vkCreateImageView(device, &image_view_ci, NULL, &swapchainImageViews[i]);
        assert(result == VK_SUCCESS);
    }
}

bool memoryTypeFromProperties(VkPhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex)
{
    for (uint32_t i = 0; i < 32; i++)
    {
        if ((typeBits & 1) == 1)
        {
            if ((memoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask)
            {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    return false;
}

void create_vulkan_DepthBuffer()
{
    depthFormat = VK_FORMAT_D16_UNORM;
    VkImageCreateInfo image_ci = {};
    vkGetPhysicalDeviceFormatProperties(gpus[0], depthFormat, &depthFormatProps);
    if (depthFormatProps.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        image_ci.tiling = VK_IMAGE_TILING_LINEAR;
        printf("tiling为VK_IMAGE_TILING_LINEAR!\n");
    }
    else if (depthFormatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        image_ci.tiling = VK_IMAGE_TILING_OPTIMAL;
        printf("tiling为VK_IMAGE_TILING_OPTIMAL!\n");
    }
    else
    {
        printf("不支持VK_FORMAT_D16_UNORM!\n");
    }
    image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_ci.pNext = NULL;
    image_ci.imageType = VK_IMAGE_TYPE_2D;
    image_ci.format = depthFormat;
    image_ci.extent.width = screenWidth;
    image_ci.extent.height = screenHeight;
    image_ci.extent.depth = 1;
    image_ci.mipLevels = 1;
    image_ci.arrayLayers = 1;
    image_ci.samples = VK_SAMPLE_COUNT_1_BIT;
    image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_ci.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_ci.queueFamilyIndexCount = 0;
    image_ci.pQueueFamilyIndices = NULL;
    image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_ci.flags = 0;

    VkMemoryAllocateInfo mem_alloc_info = {};
    mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc_info.pNext = NULL;
    mem_alloc_info.allocationSize = 0;
    mem_alloc_info.memoryTypeIndex = 0;

    VkImageViewCreateInfo depth_view_ci = {};
    depth_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depth_view_ci.pNext = NULL;
    depth_view_ci.image = VK_NULL_HANDLE;
    depth_view_ci.format = depthFormat;
    depth_view_ci.components.r = VK_COMPONENT_SWIZZLE_R;
    depth_view_ci.components.g = VK_COMPONENT_SWIZZLE_G;
    depth_view_ci.components.b = VK_COMPONENT_SWIZZLE_B;
    depth_view_ci.components.a = VK_COMPONENT_SWIZZLE_A;
    depth_view_ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depth_view_ci.subresourceRange.baseMipLevel = 0;
    depth_view_ci.subresourceRange.levelCount = 1;
    depth_view_ci.subresourceRange.baseArrayLayer = 0;
    depth_view_ci.subresourceRange.layerCount = 1;
    depth_view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depth_view_ci.flags = 0;
    VkResult result = vkCreateImage(device, &image_ci, NULL, &depthImage);
    assert(result == VK_SUCCESS);
    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(device, depthImage, &mem_reqs);
    mem_alloc_info.allocationSize = mem_reqs.size;
    VkFlags requirements_mask = 0;
    bool flag = memoryTypeFromProperties(memoryproperties, mem_reqs.memoryTypeBits, requirements_mask, &mem_alloc_info.memoryTypeIndex);
    assert(flag);
    printf("确定内存类型成功类型索引为%d\n", mem_alloc_info.memoryTypeIndex);
    result = vkAllocateMemory(device, &mem_alloc_info, NULL, &memDepth);
    assert(result == VK_SUCCESS);
    result = vkBindImageMemory(device, depthImage, memDepth, 0);
    assert(result == VK_SUCCESS);
    depth_view_ci.image = depthImage;
    result = vkCreateImageView(device, &depth_view_ci, NULL, &depthImageView);
    assert(result == VK_SUCCESS);
}

void create_render_pass()
{
    VkSemaphoreCreateInfo semaphore_ci = {};
    semaphore_ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO; // image required semaphore
    semaphore_ci.pNext = NULL;
    semaphore_ci.flags = 0;
    VkResult result = vkCreateSemaphore(device, &semaphore_ci, NULL, &imageAcquiredSemaphore);
    assert(result == VK_SUCCESS);
    VkAttachmentDescription attachments_desc[2];
    attachments_desc[0].format = formats[0];
    attachments_desc[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments_desc[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;              // color attachment
    attachments_desc[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;            // color attachment
    attachments_desc[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;   // template attachment
    attachments_desc[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // template attachment
    attachments_desc[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments_desc[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments_desc[0].flags = 0;

    attachments_desc[1].format = depthFormat;
    attachments_desc[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments_desc[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments_desc[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments_desc[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments_desc[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments_desc[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments_desc[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments_desc[1].flags = 0;

    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_reference = {};
    depth_reference.attachment = 1;
    depth_reference.layout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_desc = {};
    subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_desc.flags = 0;
    subpass_desc.inputAttachmentCount = 0;
    subpass_desc.pInputAttachments = NULL;
    subpass_desc.colorAttachmentCount = 1;
    subpass_desc.pColorAttachments = &color_reference;
    subpass_desc.pResolveAttachments = NULL;
    subpass_desc.pDepthStencilAttachment = &depth_reference;
    subpass_desc.preserveAttachmentCount = 0;
    subpass_desc.pResolveAttachments = NULL;

    VkRenderPassCreateInfo render_pass_ci = {};
    render_pass_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_ci.pNext = NULL;
    render_pass_ci.attachmentCount = 2;
    render_pass_ci.pAttachments = attachments_desc;
    render_pass_ci.subpassCount = 1;
    render_pass_ci.pSubpasses = &subpass_desc;
    render_pass_ci.dependencyCount = 0;
    render_pass_ci.pDependencies = NULL;

    result = vkCreateRenderPass(device, &render_pass_ci, NULL, &renderPass);
    assert(result == VK_SUCCESS);

    clear_values[0].color.float32[0] = 0.2f;
    clear_values[0].color.float32[1] = 0.2f;
    clear_values[0].color.float32[2] = 0.2f;
    clear_values[0].color.float32[3] = 0.2f;

    clear_values[1].depthStencil.depth = 1.0f;
    clear_values[1].depthStencil.stencil = 0;

    rp_begin_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_begin_ci.pNext = NULL;
    rp_begin_ci.renderPass = renderPass;
    rp_begin_ci.renderArea.offset.x = 0;
    rp_begin_ci.renderArea.offset.y = 0;
    rp_begin_ci.renderArea.extent.width = screenWidth;
    rp_begin_ci.renderArea.extent.height = screenHeight;
    rp_begin_ci.clearValueCount = 2;
    rp_begin_ci.pClearValues = clear_values;
}

void create_frame_buffer()
{
    VkImageView attachments_iv[2];
    attachments_iv[1] = depthImageView;
    VkFramebufferCreateInfo frame_buffer_ci = {};
    frame_buffer_ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_ci.pNext = NULL;
    frame_buffer_ci.renderPass = renderPass;
    frame_buffer_ci.attachmentCount = 2;
    frame_buffer_ci.pAttachments = attachments_iv;
    frame_buffer_ci.width = screenWidth;
    frame_buffer_ci.height = screenHeight;
    frame_buffer_ci.layers = 1;
    framebuffers = (VkFramebuffer *)malloc(swapchainImageCount * sizeof(VkFramebuffer));
    assert(framebuffers);
    for (size_t i = 0; i < swapchainImageCount; i++)
    {
        attachments_iv[0] = swapchainImageViews[i];
        VkResult result = vkCreateFramebuffer(device, &frame_buffer_ci, NULL, &framebuffers[i]);
        assert(result == VK_SUCCESS);
        printf("创建帧缓冲%ld成功\n", i);
    }
}

void createDrawableObject()
{
    int vCount = 3;
    int dataByteCount = vCount * 6 * sizeof(float);
    float *vdata = new float[vCount * 6]{
        0, 75, 0, 1, 0, 0,
        -45, 0, 0, 0, 1, 0,
        45, 0, 0, 0, 0, 1};
    drawable = new DrawableObject(vdata, dataByteCount, vCount, device, memoryproperties);
}

#ifdef _WIN32
static void run(struct window_info *info)
{ /* Placeholder for samples that want to show dynamic content */
}

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    struct window_info *info = reinterpret_cast<struct window_info *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        run(info);
        return 0;
    default:
        break;
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void init_window(struct window_info &info)
{
    WNDCLASSEX win_class;
    assert(info.width > 0);
    assert(info.height > 0);

    info.connection = GetModuleHandle(NULL);
    sprintf(info.name, "Sample");

    // Initialize the window class structure:
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WndProc;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = info.connection; // hInstance
    win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    win_class.lpszMenuName = NULL;
    win_class.lpszClassName = info.name;
    win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    // Register window class:
    if (!RegisterClassEx(&win_class))
    {
        // It didn't work, so try to give a useful error:
        printf("Unexpected error trying to start the application!\n");
        fflush(stdout);
        exit(1);
    }
    // Create window with the registered class:
    RECT wr = {0, 0, info.width, info.height};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    info.window = CreateWindowEx(0,
                                 info.name,            // class name
                                 info.name,            // app name
                                 WS_OVERLAPPEDWINDOW | // window style
                                     WS_VISIBLE | WS_SYSMENU,
                                 100, 100,           // x/y coords
                                 wr.right - wr.left, // width
                                 wr.bottom - wr.top, // height
                                 NULL,               // handle to parent
                                 NULL,               // handle to menu
                                 info.connection,    // hInstance
                                 NULL);              // no extra parameters
    if (!info.window)
    {
        // It didn't work, so try to give a useful error:
        printf("Cannot create a window in which to draw!\n");
        fflush(stdout);
        exit(1);
    }
    SetWindowLongPtr(info.window, GWLP_USERDATA, (LONG_PTR)&info);
}

void destroy_window(struct window_info &info)
{
    vkDestroySurfaceKHR(info.inst, info.surface, NULL);
    DestroyWindow(info.window);
}

#elif defined(VK_USE_PLATFORM_METAL_EXT)

// iOS & macOS: init_window() implemented externally to allow access to Objective-C components

void destroy_window(struct window_info &info) { info.caMetalLayer = NULL; }

#elif defined(__ANDROID__)
// Android implementation.
void init_window(struct window_info &info) {}

void destroy_window(struct window_info &info) {}

#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)

void init_window(struct window_info &info)
{
    assert(info.width > 0);
    assert(info.height > 0);

    info.window = wl_compositor_create_surface(info.compositor);
    if (!info.window)
    {
        printf("Can not create wayland_surface from compositor!\n");
        fflush(stdout);
        exit(1);
    }

    info.shell_surface = wl_shell_get_shell_surface(info.shell, info.window);
    if (!info.shell_surface)
    {
        printf("Can not get shell_surface from wayland_surface!\n");
        fflush(stdout);
        exit(1);
    }

    wl_shell_surface_add_listener(info.shell_surface, &shell_surface_listener, &info);
    wl_shell_surface_set_toplevel(info.shell_surface);
}

void destroy_window(struct window_info &info)
{
    wl_shell_surface_destroy(info.shell_surface);
    wl_surface_destroy(info.window);
    wl_shell_destroy(info.shell);
    wl_compositor_destroy(info.compositor);
    wl_registry_destroy(info.registry);
    wl_display_disconnect(info.display);
}

#else

void init_window(struct window_info &info)
{
    assert(info.width > 0);
    assert(info.height > 0);

    uint32_t value_mask, value_list[32];

    info.window = xcb_generate_id(info.connection);

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = info.screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window(info.connection, XCB_COPY_FROM_PARENT, info.window, info.screen->root, 0, 0, info.width, info.height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, info.screen->root_visual, value_mask, value_list);

    /* Magic code that will send notification when window is destroyed */
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(info.connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(info.connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(info.connection, 0, 16, "WM_DELETE_WINDOW");
    info.atom_wm_delete_window = xcb_intern_atom_reply(info.connection, cookie2, 0);

    xcb_change_property(info.connection, XCB_PROP_MODE_REPLACE, info.window, (*reply).atom, 4, 32, 1,
                        &(*info.atom_wm_delete_window).atom);
    free(reply);

    xcb_map_window(info.connection, info.window);

    // Force the x/y coordinates to 100,100 results are identical in consecutive
    // runs
    const uint32_t coords[] = {100, 100};
    xcb_configure_window(info.connection, info.window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
    xcb_flush(info.connection);

    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event(info.connection)))
    {
        if ((e->response_type & ~0x80) == XCB_EXPOSE)
            break;
    }
}

void destroy_window(struct window_info &info)
{
    // vkDestroySurfaceKHR(info.inst, info.surface, NULL);
    xcb_destroy_window(info.connection, info.window);
    xcb_disconnect(info.connection);
}

#endif // _WIN32

void init_window_size(struct window_info &info, int32_t default_width, int32_t default_height)
{
#ifdef __ANDROID__
    AndroidGetWindowSize(&info.width, &info.height);
#else
    info.width = default_width;
    info.height = default_height;
#endif
}

void init_connection(struct window_info &info)
{
#if defined(VK_USE_PLATFORM_XCB_KHR)
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    info.connection = xcb_connect(NULL, &scr);
    if (info.connection == NULL || xcb_connection_has_error(info.connection))
    {
        std::cout << "Unable to make an XCB connection\n";
        exit(-1);
    }

    setup = xcb_get_setup(info.connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0)
        xcb_screen_next(&iter);

    info.screen = iter.data;
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    info.display = wl_display_connect(nullptr);

    if (info.display == nullptr)
    {
        printf(
            "Cannot find a compatible Vulkan installable client driver "
            "(ICD).\nExiting ...\n");
        fflush(stdout);
        exit(1);
    }

    info.registry = wl_display_get_registry(info.display);
    wl_registry_add_listener(info.registry, &registry_listener, &info);
    wl_display_dispatch(info.display);
#endif
}
