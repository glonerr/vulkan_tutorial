#ifndef VULKAN_MANAGER_H
#define VULKAN_MANAGER_H
#include "vulkan_wrapper.h" //for android and unix etc most platform

#include <vector>

/*
 * Keep each of our swap chain buffers' image, command buffer and view in one
 * spot
 */
typedef struct _swap_chain_buffers
{
    VkImage image;
    VkImageView view;
} swap_chain_buffer;

/*
 * A layer can expose extensions, keep track of those
 * extensions here.
 */
typedef struct
{
    VkLayerProperties properties;
    std::vector<VkExtensionProperties> instance_extensions;
    std::vector<VkExtensionProperties> device_extensions;
} layer_properties;

/*
 * Structure for tracking information used / created / modified
 * by utility functions.
 */
struct window_info
{
#ifdef _WIN32
#define APP_NAME_STR_LEN 80
    HINSTANCE connection;        // hInstance - Windows Instance
    char name[APP_NAME_STR_LEN]; // Name to put on the window/icon
    HWND window;                 // hWnd - window handle
#elif defined(VK_USE_PLATFORM_METAL_EXT)
    void *caMetalLayer;
#elif defined(__ANDROID__)
    PFN_vkCreateAndroidSurfaceKHR fpCreateAndroidSurfaceKHR;
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    wl_display *display;
    wl_registry *registry;
    wl_compositor *compositor;
    wl_surface *window;
    wl_shell *shell;
    wl_shell_surface *shell_surface;
#else
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_intern_atom_reply_t *atom_wm_delete_window;
#endif // _WIN32
    int width, height;
};

std::vector<const char *> instanceExtensionNames;
VkInstance instance;
uint32_t gpuCount;
std::vector<VkPhysicalDevice> gpus;
uint32_t queueFamilyCount;
std::vector<VkQueueFamilyProperties> queueFamilyProps;
uint32_t queueGraphicsFamilyIndex;
VkQueue queueGraphics;
uint32_t queuePresentFamilyIndex;
std::vector<const char *> deviceExtensionName;
VkDevice device;
VkCommandPool cmdPool;
VkCommandBuffer cmdBuffer;
VkCommandBufferBeginInfo cmd_buf_info;
VkCommandBuffer cmd_bufs[1];
VkSubmitInfo submit_info[1];
uint32_t screenWidth;
uint32_t screenHeight;
VkSurfaceKHR surface;
std::vector<VkFormat> formats;
VkSurfaceCapabilitiesKHR surfCapabilities;
uint32_t presentModeCount;
std::vector<VkPresentModeKHR> presentModes;
VkExtent2D swapchainExtent;
VkSwapchainKHR swapChain;
uint32_t swapchainImageCount;
std::vector<VkImage> swapchainImages;
std::vector<VkImageView> swapchainImageViews;
VkFormat depthFormat;
VkFormatProperties depthFormatProps;
VkImage depthImage;
VkPhysicalDeviceMemoryProperties memoryproperties;
VkDeviceMemory memDepth;
VkImageView depthImageView;
VkSemaphore imageAcquiredSemaphore;
uint32_t currentBuffer;
VkRenderPass renderPass;
VkClearValue clear_values[2];
VkRenderPassBeginInfo rp_begin;
VkFence taskFinishFench;
VkPresentInfoKHR present;
VkFramebuffer *framebuffers;
float xAngle;

void init_window_size(struct window_info &info, int32_t default_width, int32_t default_height);
void init_window(struct window_info &info);
void init_connection(struct window_info &info);
void destroy_window(struct window_info &info);

void init_vulkan_instance();
void enumerate_vulkan_phy_device();
void create_vulkan_device();
void create_vulkan_CommandBuffer();
void init_queue();
void create_vulkan_swapchain();
void create_vulkan_DepthBuffer();
void create_render_pass();
void create_frame_buffer();
void createDrawableObject();
void drawObject();
void doVulkan();
void initPipeline();
void createFence();
void initPresentInfo();
void initMatrix();
void flushUniformBuffer();
void flushTexToDecSet();
void destroy_window();
void destroyFence();
void destroyPipeline();
void destroyDrawableObject();
void destroy_frame_buffer();
void destroy_render_pass();
void destroy_vulkan_DepthBuffer();
void destroy_vulkan_swapchain();
void destroy_vulkan_CommandBuffer();
void destroy_vulkan_device();
void destroy_vulkan_instance();
#endif // !VULKAN_MANAGER_H