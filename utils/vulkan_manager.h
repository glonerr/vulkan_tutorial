#ifndef VULKAN_MANAGER_H
#define VULKAN_MANAGER_H
#ifdef USE_VULKAN_WRAPPER
#include "vulkan_wrapper.h"
#else
#include <vulkan/vulkan.h>
#endif
#include "drawable_object.h"
#include "shader_queue_suit.h"

#include <vector>

VkInstance instance;
uint32_t gpuCount;
std::vector<VkPhysicalDevice> gpus;
uint32_t queueFamilyCount;
std::vector<VkQueueFamilyProperties> queueFamilyProps;
uint32_t queueGraphicsFamilyIndex;
VkQueue queueGraphics;
uint32_t queuePresentFamilyIndex;
std::vector<const char *> deviceExtensionName;
VkCommandPool cmdPool;
VkCommandBuffer cmdBuffer;
VkCommandBufferBeginInfo cmd_buf_info;
VkCommandBuffer cmd_bufs[1];
VkSubmitInfo submit_info[1];
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
VkRenderPassBeginInfo rp_begin_ci;
VkFence taskFinishFench;
VkPresentInfoKHR present;
VkFramebuffer *framebuffers;
float xAngle;

DrawableObject *drawable;
ShaderQueueSuit *sqsCL;


VkBool32 demo_check_layers(const std::vector<layer_properties> &layer_props, const std::vector<const char *> &layer_names);
void init_window_size(struct window_info &info, int32_t default_width, int32_t default_height);
void init_window(struct window_info &info);
void init_connection(struct window_info &info);
void destroy_window(struct window_info &info);
void init_vulkan(struct window_info &info);

VkResult init_device_extension_properties(struct window_info &info, layer_properties &layer_props);
VkResult init_global_layer_properties(struct window_info &info);

void init_instance_extension_names(struct window_info &info);
void init_vulkan_instance(struct window_info &info);
void enumerate_vulkan_phy_device();
void create_vulkan_device(struct window_info &info);
void create_vulkan_CommandBuffer(struct window_info &info);
void init_queue(struct window_info &info);
void create_vulkan_swapchain(struct window_info &info);
void create_vulkan_DepthBuffer(struct window_info &info);
void create_render_pass(struct window_info &info);
void create_frame_buffer(struct window_info &info);
void createDrawableObject(struct window_info &info);
void drawObject(struct window_info &info);
void doVulkan();
void initPipeline(struct window_info &info);
void createFence(struct window_info &info);
void initPresentInfo();
void initMatrix(struct window_info &info);
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