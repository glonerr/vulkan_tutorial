cmake_minimum_required(VERSION 3.0.0)

project(vulkan_tutorial)

set(MAJOR "1")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/home/lonerr/Tools/systools/vulkan_sdk/1.2.131.2/source/Vulkan-Tools/cmake")

set(CMAKE_INSTALL_PREFIX "")

set(CMAKE_BUILD_TYPE Debug)

if(NOT WIN32)
        include(FindPkgConfig)
        option(BUILD_WSI_XCB_SUPPORT "Build XCB WSI support" ON)
        option(BUILD_WSI_WAYLAND_SUPPORT "Build Wayland WSI support" OFF)
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

message(STATUS ${CMAKE_SOURCE_DIR})

set(UTILS_NAME vkutils)

add_subdirectory(utils)

file(GLOB_RECURSE source vulkan_tutorial_01.cpp)

message(STATUS ${XCB_INCLUDE_DIRS} ${XCB_LIBRARIES})

include_directories(${XCB_INCLUDE_DIRS} "./")

link_libraries(${XCB_LIBRARIES} m)

add_definitions(-DVK_USE_PLATFORM_XCB_KHR)

add_executable(vulkan_tutorial_01 ${source})

target_link_libraries(vulkan_tutorial_01 ${UTILS_NAME} ${CMAKE_DL_LIBS})
