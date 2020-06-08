file(GLOB UTILS_SOURCE vulkan_manager.cpp vulkan_wrapper.cpp drawable_object.cpp)

if(NOT WIN32)
        include(FindPkgConfig)
        option(BUILD_WSI_XCB_SUPPORT "Build XCB WSI support" ON)
        option(BUILD_WSI_WAYLAND_SUPPORT "Build Wayland WSI support" OFF)
        add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
        set(DEMOS_WSI_SELECTION
            "XCB"
            CACHE STRING "Select WSI target for demos (XCB, XLIB, WAYLAND, DISPLAY)")
        set(SAMPLES_WSI_SELECTION
            "XCB"
            CACHE STRING "Select WSI target for api-samples (XCB, WAYLAND, DISPLAY)")

        if(BUILD_WSI_XCB_SUPPORT)
                find_package(XCB REQUIRED)
        endif()
endif()

include_directories(${XCB_INCLUDE_DIRS})

link_libraries(${XCB_LIBRARIES} m)

add_library(${UTILS_NAME} SHARED ${UTILS_SOURCE})

SET_TARGET_PROPERTIES(${UTILS_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)