#include "utils/vulkan_manager.h"
#include <stdlib.h>
int main()
{
    struct window_info info = {};
    init_window_size(info, 500, 500);
    init_connection(info);
    init_window(info);

    InitVulkan();
    init_vulkan_instance();
    enumerate_vulkan_phy_device();
    create_vulkan_device();
    create_vulkan_CommandBuffer();
    init_queue();
    create_vulkan_swapchain(info);
    create_vulkan_DepthBuffer();
    create_render_pass();
    create_frame_buffer();
    createDrawableObject();
    initPipeline();

    // close connection to server
    xcb_disconnect(info.connection);
    destroy_window(info);
    return 0;
}
