#include "utils/vulkan_manager.h"
#include <stdlib.h>
#include <iostream>
int main()
{
    struct window_info info = {};
    
    init_vulkan(info);
    init_global_layer_properties(info);
    /* Enable validation*/
    info.instance_layer_names.push_back("VK_LAYER_KHRONOS_validation");
    if (!demo_check_layers(info.instance_layer_properties, info.instance_layer_names)) {
        if (!demo_check_layers(info.instance_layer_properties, info.instance_layer_names)) {
            std::cout << "Set the environment variable VK_LAYER_PATH to point to the location of your layers" << std::endl;
            exit(1);
        }
    }

    /* Enable debug callback extension */
    info.instance_extension_names.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    init_vulkan_instance(info);
    enumerate_vulkan_phy_device();
    init_window_size(info, 500, 500);
    init_connection(info);
    init_window(info);
    create_vulkan_device(info);
    create_vulkan_CommandBuffer(info);
    init_queue(info);
    create_vulkan_swapchain(info);
    create_vulkan_DepthBuffer(info);
    create_render_pass(info);
    create_frame_buffer(info);
    createDrawableObject(info);
    initPipeline(info);
    createFence(info);
    initPresentInfo();
    initMatrix(info);
    drawObject(info);


    // close connection to server
    xcb_disconnect(info.connection);
    destroy_window(info);
    return 0;
}
