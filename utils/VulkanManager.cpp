#include "VulkanManager.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

std::vector<const char *> VulkanManager::instanceExtensionNames;
VkInstance VulkanManager::instance;
uint32_t VulkanManager::gpuCount;
std::vector<VkPhysicalDevice> VulkanManager::gpus;
VkPhysicalDeviceMemoryProperties VulkanManager::memoryproperties;
std::vector<VkQueueFamilyProperties> VulkanManager::queueFamilyProps;
uint32_t VulkanManager::queueFamilyCount;
uint32_t VulkanManager::queueGraphicsFamilyIndex;
VkQueue VulkanManager::queueGraphics;
VkDevice VulkanManager::device;
std::vector<const char *> VulkanManager::deviceExtensionName;
VkCommandPool VulkanManager::cmdPool;
VkCommandBuffer VulkanManager::cmdBuffer;
VkCommandBufferBeginInfo VulkanManager::cmd_buf_info;
VkCommandBuffer VulkanManager::cmd_bufs[1];
VkSubmitInfo VulkanManager::submit_info[1];
uint32_t VulkanManager::screenWidth;
uint32_t VulkanManager::screenHeight;
VkSurfaceKHR VulkanManager::surface;

#ifdef _WIN32
#define APP_NAME_STR_LEN 80
    HINSTANCE VulkanManager::connection;        // hInstance - Windows Instance
    char VulkanManager::name[APP_NAME_STR_LEN]; // Name to put on the window/icon
    HWND VulkanManager::window;                 // hWnd - window handle
#elif defined(VK_USE_PLATFORM_METAL_EXT)
    void *VulkanManager::caMetalLayer;
#elif defined(__ANDROID__)
    PFN_vkCreateAndroidSurfaceKHR VulkanManager::fpCreateAndroidSurfaceKHR;
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    wl_display *display;
    wl_registry *registry;
    wl_compositor *compositor;
    wl_surface *window;
    wl_shell *shell;
    wl_shell_surface *shell_surface;
#else
    xcb_connection_t *VulkanManager::connection;
    xcb_screen_t *VulkanManager::screen;
    xcb_window_t VulkanManager::window;
    xcb_intern_atom_reply_t *VulkanManager::atom_wm_delete_window;
#endif // _WIN32

void VulkanManager::init_vulkan_instance()
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

void VulkanManager::enumerate_vulkan_phy_device()
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

void VulkanManager::create_vulkan_device()
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

void VulkanManager::create_vulkan_CommandBuffer()
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
}

void VulkanManager::init_quere()
{
    vkGetDeviceQueue(device, queueGraphicsFamilyIndex, 0, &queueGraphics);
}

void VulkanManager::create_vulkan_swapchain()
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
    createInfo.connection = connection;
    createInfo.window = window;
    result = vkCreateXcbSurfaceKHR(instance, &createInfo, NULL, &surface);
#endif // __ANDROID__  && _WIN32
}

void VulkanManager::init_window_size(int32_t width, int32_t height) {
#ifdef __ANDROID__
    AndroidGetWindowSize(&screenWidth, &screenHeight);
#else
    screenWidth = width;
    screenHeight = height;
#endif
}

#ifdef _WIN32
static void run() { /* Placeholder for samples that want to show dynamic content */ }

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    struct sample_info *info = reinterpret_cast<struct sample_info *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (uMsg) {
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

void VulkanManager::init_window() {
    WNDCLASSEX win_class;
    assert(screenWidth > 0);
    assert(screenHeight > 0);

    connection = GetModuleHandle(NULL);
    sprintf(info.name, "Sample");

    // Initialize the window class structure:
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WndProc;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = connection;  // hInstance
    win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    win_class.lpszMenuName = NULL;
    win_class.lpszClassName = name;
    win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    // Register window class:
    if (!RegisterClassEx(&win_class)) {
        // It didn't work, so try to give a useful error:
        printf("Unexpected error trying to start the application!\n");
        fflush(stdout);
        exit(1);
    }
    // Create window with the registered class:
    RECT wr = {0, 0, screenWidth, screenHeight};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    window = CreateWindowEx(0,
                                 name,             // class name
                                 name,             // app name
                                 WS_OVERLAPPEDWINDOW |  // window style
                                     WS_VISIBLE | WS_SYSMENU,
                                 100, 100,            // x/y coords
                                 wr.right - wr.left,  // width
                                 wr.bottom - wr.top,  // height
                                 NULL,                // handle to parent
                                 NULL,                // handle to menu
                                 connection,     // hInstance
                                 NULL);               // no extra parameters
    if (!window) {
        // It didn't work, so try to give a useful error:
        printf("Cannot create a window in which to draw!\n");
        fflush(stdout);
        exit(1);
    }
    SetWindowLongPtr(info.window, GWLP_USERDATA, (LONG_PTR)&info);
}

void VulkanManager::destroy_window() {
    vkDestroySurfaceKHR(instance, surface, NULL);
    DestroyWindow(window);
}

#elif defined(VK_USE_PLATFORM_METAL_EXT)

// iOS & macOS: init_window() implemented externally to allow access to Objective-C components

void VulkanManager::destroy_window() { caMetalLayer = NULL; }

#elif defined(__ANDROID__)
// Android implementation.
void VulkanManager::init_window() {}

void VulkanManager::destroy_window() {}

#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)

void VulkanManager::init_window() {
    assert(screenWidth > 0);
    assert(screenHeight > 0);

    window = wl_compositor_create_surface(compositor);
    if (!window) {
        printf("Can not create wayland_surface from compositor!\n");
        fflush(stdout);
        exit(1);
    }

    shell_surface = wl_shell_get_shell_surface(shell, window);
    if (!shell_surface) {
        printf("Can not get shell_surface from wayland_surface!\n");
        fflush(stdout);
        exit(1);
    }

    wl_shell_surface_add_listener(shell_surface, &shell_surface_listener, &info);
    wl_shell_surface_set_toplevel(shell_surface);
}

void VulkanManager::destroy_window() {
    wl_shell_surface_destroy(shell_surface);
    wl_surface_destroy(window);
    wl_shell_destroy(shell);
    wl_compositor_destroy(compositor);
    wl_registry_destroy(registry);
    wl_display_disconnect(display);
}

#else

void VulkanManager::init_window() {
    assert(screenWidth > 0);
    assert(screenHeight > 0);

    uint32_t value_mask, value_list[32];

    window = xcb_generate_id(connection);

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, screenWidth, screenHeight, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, value_mask, value_list);

    /* Magic code that will send notification when window is destroyed */
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
    atom_wm_delete_window = xcb_intern_atom_reply(connection, cookie2, 0);

    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, (*reply).atom, 4, 32, 1,
                        &(*atom_wm_delete_window).atom);
    free(reply);

    xcb_map_window(connection, window);

    // Force the x/y coordinates to 100,100 results are identical in consecutive
    // runs
    const uint32_t coords[] = {100, 100};
    xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
    xcb_flush(connection);

    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event(connection))) {
        if ((e->response_type & ~0x80) == XCB_EXPOSE) break;
    }
}

void VulkanManager::destroy_window() {
    vkDestroySurfaceKHR(instance, surface, NULL);
    xcb_destroy_window(connection, window);
    xcb_disconnect(connection);
}

#endif  // _WIN32