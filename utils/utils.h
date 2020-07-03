#ifndef UTILS_H
#define UTILS_H
#ifdef USE_VULKAN_WRAPPER
#include "vulkan_wrapper.h"
#else
#include <vulkan/vulkan.h>
#endif
#include <vector>

/*
 * structure to track all objects related to a texture.
 */
struct texture_object {
    VkSampler sampler;

    VkImage image;
    VkImageLayout imageLayout;

    bool needs_staging;
    VkBuffer buffer;
    VkDeviceSize buffer_size;

    VkDeviceMemory image_memory;
    VkDeviceMemory buffer_memory;
    VkImageView view;
    int32_t tex_width, tex_height;
};

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
    VkSurfaceKHR surface;
    bool prepared;
    bool use_staging_buffer;
    bool save_images;

    std::vector<const char *> instance_layer_names;
    std::vector<const char *> instance_extension_names;
    std::vector<layer_properties> instance_layer_properties;
    std::vector<VkExtensionProperties> instance_extension_properties;
    VkInstance inst;

    std::vector<const char *> device_extension_names;
    std::vector<VkExtensionProperties> device_extension_properties;
    std::vector<VkPhysicalDevice> gpus;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    uint32_t graphics_queue_family_index;
    uint32_t present_queue_family_index;
    VkPhysicalDeviceProperties gpu_props;
    std::vector<VkQueueFamilyProperties> queue_props;
    VkPhysicalDeviceMemoryProperties memory_properties;

    VkFramebuffer *framebuffers;
    uint32_t width, height;
    VkFormat format;

    uint32_t swapchainImageCount;
    VkSwapchainKHR swap_chain;
    std::vector<swap_chain_buffer> buffers;
    VkSemaphore imageAcquiredSemaphore;

    VkCommandPool cmd_pool;

    struct
    {
        VkFormat format;

        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depth;

    std::vector<struct texture_object> textures;

    struct
    {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo buffer_info;
    } uniform_data;

    struct
    {
        VkDescriptorImageInfo image_info;
    } texture_data;

    struct
    {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo buffer_info;
    } vertex_buffer;
    VkVertexInputBindingDescription vi_binding;
    VkVertexInputAttributeDescription vi_attribs[2];

    // glm::mat4 Projection;
    // glm::mat4 View;
    // glm::mat4 Model;
    // glm::mat4 Clip;
    // glm::mat4 MVP;

    VkCommandBuffer cmd; // Buffer for initialization commands
    VkPipelineLayout pipeline_layout;
    std::vector<VkDescriptorSetLayout> desc_layout;
    VkPipelineCache pipelineCache;
    VkRenderPass render_pass;
    VkPipeline pipeline;

    VkPipelineShaderStageCreateInfo shaderStages[2];

    VkDescriptorPool desc_pool;
    std::vector<VkDescriptorSet> desc_set;

    PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;
    PFN_vkDebugReportMessageEXT dbgBreakCallback;
    std::vector<VkDebugReportCallbackEXT> debug_report_callbacks;

    uint32_t current_buffer;
    uint32_t queue_family_count;

    VkViewport viewport;
    VkRect2D scissor;
};

bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader,
               std::vector<unsigned int> &spirv);
void init_glslang();
void finalize_glslang();
/* 管线布置创作，创作和描述符设置布局数 */
#define NUM_DESCRIPTOR_SETS 1
#endif