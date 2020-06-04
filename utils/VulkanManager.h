#ifndef VULKANMANAGER_H
#define VULKANMANAGER_H
#define FENCE_TIMEOUT 100000000

// define platform for vulkan
#include <vulkan/vulkan.h>
#include <vector>

class VulkanManager
{
public:
#ifdef _WIN32
#define APP_NAME_STR_LEN 80
    static HINSTANCE connection;        // hInstance - Windows Instance
    static char name[APP_NAME_STR_LEN]; // Name to put on the window/icon
    static HWND window;                 // hWnd - window handle
#elif defined(VK_USE_PLATFORM_METAL_EXT)
    static void *caMetalLayer;
#elif defined(__ANDROID__)
    static PFN_vkCreateAndroidSurfaceKHR fpCreateAndroidSurfaceKHR;
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    static wl_display *display;
    static wl_registry *registry;
    static wl_compositor *compositor;
    static wl_surface *window;
    static wl_shell *shell;
    static wl_shell_surface *shell_surface;
#else
    static xcb_connection_t *connection;
    static xcb_screen_t *screen;
    static xcb_window_t window;
    static xcb_intern_atom_reply_t *atom_wm_delete_window;
#endif // _WIN32
    static std::vector<const char *> instanceExtensionNames;
    static VkInstance instance;
    static uint32_t gpuCount;
    static std::vector<VkPhysicalDevice> gpus;
    static uint32_t queueFamilyCount;
    static std::vector<VkQueueFamilyProperties> queueFamilyProps;
    static uint32_t queueGraphicsFamilyIndex;
    static VkQueue queueGraphics;
    static uint32_t queuePresentFamilyIndex;
    static std::vector<const char *> deviceExtensionName;
    static VkDevice device;
    static VkCommandPool cmdPool;
    static VkCommandBuffer cmdBuffer;
    static VkCommandBufferBeginInfo cmd_buf_info;
    static VkCommandBuffer cmd_bufs[1];
    static VkSubmitInfo submit_info[1];
    static uint32_t screenWidth;
    static uint32_t screenHeight;
    static VkSurfaceKHR surface;
    static std::vector<VkFormat> formats;
    static VkSurfaceCapabilitiesKHR surfCapabilities;
    static uint32_t presentModeCount;
    static std::vector<VkPresentModeKHR> presentModes;
    static VkExtent2D swapchainExtent;
    static VkSwapchainKHR swapChain;
    static uint32_t swapchainImageCount;
    static std::vector<VkImage> swapchainImages;
    static std::vector<VkImageView> swapchainImageViews;
    static VkFormat depthFormat;
    static VkFormatProperties depthFormatProps;
    static VkImage depthImage;
    static VkPhysicalDeviceMemoryProperties memoryproperties;
    static VkDeviceMemory memDepth;
    static VkImageView depthImageView;
    static VkSemaphore imageAcquiredSemaphore;
    static uint32_t currentBuffer;
    static VkRenderPass renderPass;
    static VkClearValue clear_values[2];
    static VkRenderPassBeginInfo rp_begin;
    static VkFence taskFinishFench;
    static VkPresentInfoKHR present;
    static VkFramebuffer *framebuffers;
    static float xAngle;

    static void init_window_size(int32_t width, int32_t height);
    static void init_window();
    static void init_vulkan_instance();
    static void enumerate_vulkan_phy_device();
    static void create_vulkan_device();
    static void create_vulkan_CommandBuffer();
    static void init_quere();
    static void create_vulkan_swapchain();
    static void create_vulkan_DepthBuffer();
    static void create_render_pass();
    static void create_frame_buffer();
    static void createDrawableObject();
    static void drawObject();
    static void doVulkan();
    static void initPipeline();
    static void createFence();
    static void initPresentInfo();
    static void initMatrix();
    static void flushUniformBuffer();
    static void flushTexToDecSet();
    static void destroy_window();
    static void destroyFence();
    static void destroyPipeline();
    static void destroyDrawableObject();
    static void destroy_frame_buffer();
    static void destroy_render_pass();
    static void destroy_vulkan_DepthBuffer();
    static void destroy_vulkan_swapchain();
    static void destroy_vulkan_CommandBuffer();
    static void destroy_vulkan_device();
    static void destroy_vulkan_instance();
};
#endif