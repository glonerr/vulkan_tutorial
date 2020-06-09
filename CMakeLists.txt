cmake_minimum_required(VERSION 3.10.2)

project(vulkan_tutorial)

set(MAJOR "1")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/home/lonerr/Tools/systools/vulkan_sdk/1.2.131.2/source/Vulkan-Tools/cmake")

set(CMAKE_INSTALL_PREFIX "")

set(CMAKE_BUILD_TYPE Debug)

find_program(GLSLANG_VALIDATOR NAMES glslangValidator
             HINTS "${CMAKE_SOURCE_DIR}/../x86_64/bin"
	           "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/install/bin"
                   "${PROJECT_SOURCE_DIR}/../${BINDATA_DIR}" )

find_path(GLSLANG_SPIRV_INCLUDE_DIR SPIRV/spirv.hpp HINTS "${CMAKE_SOURCE_DIR}/../x86_64/include"
                                                          "${CMAKE_SOURCE_DIR}/../source/glslang/"
                                                          "${CMAKE_SOURCE_DIR}/../glslang"
                                                          DOC "Path to SPIRV/spirv.hpp")
find_path(SPIRV_TOOLS_INCLUDE_DIR spirv-tools/libspirv.h HINTS "${CMAKE_SOURCE_DIR}/../x86_64/include"
				                               "${CMAKE_SOURCE_DIR}/../spirv-tools/include"
                                                               "${CMAKE_SOURCE_DIR}/../source/spirv-tools/include"
                                                               "${CMAKE_SOURCE_DIR}/../spirv-tools/external/include"
                                                               "${CMAKE_SOURCE_DIR}/../source/spirv-tools/external/include"
                                                               DOC "Path to spirv-tools/libspirv.h")

if (WIN32)
    set (GLSLANG_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/glslang/Release"
                             "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/glslang/OSDependent/Windows/Release"
                             "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/hlsl/Release"
                             "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/OGLCompilersDLL/Release"
                             "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/SPIRV/Release" )
    set (SPIRV_TOOLS_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../spirv-tools/${BUILDTGT_DIR}/source/Release")
    set (SPIRV_TOOLS_OPT_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../spirv-tools/${BUILDTGT_DIR}/source/opt/Release")
else()
    set (GLSLANG_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../x86_64/lib" "${CMAKE_SOURCE_DIR}/../glslang/build/install/lib" "${CMAKE_SOURCE_DIR}/../x86_64/lib/glslang" )
    set (SPIRV_TOOLS_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../x86_64/lib" "${CMAKE_SOURCE_DIR}/../spirv-tools/build" "${CMAKE_SOURCE_DIR}/../x86_64/lib/spirv-tools" )
    set (SPIRV_TOOLS_OPT_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../x86_64/lib" "${SPIRV_TOOLS_SEARCH_PATH}")
endif()

find_library(GLSLANG_LIB NAMES glslang
             HINTS ${GLSLANG_SEARCH_PATH} )

find_library(OGLCompiler_LIB NAMES OGLCompiler
             HINTS ${GLSLANG_SEARCH_PATH} )

find_library(OSDependent_LIB NAMES OSDependent
             HINTS ${GLSLANG_SEARCH_PATH} )

find_library(HLSL_LIB NAMES HLSL
             HINTS ${GLSLANG_SEARCH_PATH} )

find_library(SPIRV_LIB NAMES SPIRV
             HINTS ${GLSLANG_SEARCH_PATH} )

find_library(SPIRV_TOOLS_LIB NAMES SPIRV-Tools
             HINTS ${SPIRV_TOOLS_SEARCH_PATH} )

find_library(SPIRV_TOOLS_OPT_LIB NAMES SPIRV-Tools-opt
             HINTS ${SPIRV_TOOLS_OPT_SEARCH_PATH} )

find_library(SPIRV_REMAPPER_LIB NAMES SPVRemapper
             HINTS ${GLSLANG_SEARCH_PATH} )

# On Windows, we must pair Debug and Release appropriately
if (WIN32)
    set (GLSLANG_DEBUG_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/glslang/Debug"
                                   "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/glslang/OSDependent/Windows/Debug"
                                   "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/hlsl/Debug"
                                   "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/OGLCompilersDLL/Debug"
                                   "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/SPIRV/Debug")
    set (SPIRV_TOOLS_DEBUG_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../spirv-tools/${BUILDTGT_DIR}/source/Debug")
    set (SPIRV_TOOLS_OPT_DEBUG_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../spirv-tools/${BUILDTGT_DIR}/source/opt/Debug")

    add_library(glslang     STATIC IMPORTED)
    add_library(OGLCompiler STATIC IMPORTED)
    add_library(OSDependent STATIC IMPORTED)
    add_library(HLSL        STATIC IMPORTED)
    add_library(SPIRV       STATIC IMPORTED)
    add_library(Loader      STATIC IMPORTED)
    add_library(SPIRV-Tools STATIC IMPORTED)
    add_library(SPIRV-Tools-opt STATIC IMPORTED)
    add_library(SPVRemapper STATIC IMPORTED)

    find_library(GLSLANG_DLIB NAMES glslangd
                 HINTS ${GLSLANG_DEBUG_SEARCH_PATH} )
    find_library(OGLCompiler_DLIB NAMES OGLCompilerd
                 HINTS ${GLSLANG_DEBUG_SEARCH_PATH} )
    find_library(OSDependent_DLIB NAMES OSDependentd
                 HINTS ${GLSLANG_DEBUG_SEARCH_PATH} )
    find_library(HLSL_DLIB NAMES HLSLd
                 HINTS ${GLSLANG_DEBUG_SEARCH_PATH} )
    find_library(SPIRV_DLIB NAMES SPIRVd
                 HINTS ${GLSLANG_DEBUG_SEARCH_PATH} )
    find_library(SPIRV_TOOLS_DLIB NAMES SPIRV-Tools
                 HINTS ${SPIRV_TOOLS_DEBUG_SEARCH_PATH} )
    find_library(SPIRV_TOOLS_OPT_DLIB NAMES SPIRV-Tools-opt
                 HINTS ${SPIRV_TOOLS_OPT_DEBUG_SEARCH_PATH} )
    find_library(SPIRV_REMAPPER_DLIB NAMES SPVRemapperd
                 HINTS ${GLSLANG_DEBUG_SEARCH_PATH} )

    set_target_properties(glslang PROPERTIES
                         IMPORTED_LOCATION       "${GLSLANG_LIB}"
                         IMPORTED_LOCATION_DEBUG "${GLSLANG_DLIB}")
    set_target_properties(OGLCompiler PROPERTIES
                         IMPORTED_LOCATION       "${OGLCompiler_LIB}"
                         IMPORTED_LOCATION_DEBUG "${OGLCompiler_DLIB}")
    set_target_properties(OSDependent PROPERTIES
                         IMPORTED_LOCATION       "${OSDependent_LIB}"
                         IMPORTED_LOCATION_DEBUG "${OSDependent_DLIB}")
    set_target_properties(HLSL PROPERTIES
                         IMPORTED_LOCATION       "${HLSL_LIB}"
                         IMPORTED_LOCATION_DEBUG "${HLSL_DLIB}")
    set_target_properties(SPIRV PROPERTIES
                         IMPORTED_LOCATION       "${SPIRV_LIB}"
                         IMPORTED_LOCATION_DEBUG "${SPIRV_DLIB}")
    set_target_properties(SPIRV-Tools PROPERTIES
                         IMPORTED_LOCATION       "${SPIRV_TOOLS_LIB}"
                         IMPORTED_LOCATION_DEBUG "${SPIRV_TOOLS_DLIB}")
    set_target_properties(SPIRV-Tools-opt PROPERTIES
                         IMPORTED_LOCATION       "${SPIRV_TOOLS_OPT_LIB}"
                         IMPORTED_LOCATION_DEBUG "${SPIRV_TOOLS_OPT_DLIB}")
    set_target_properties(SPVRemapper PROPERTIES
                         IMPORTED_LOCATION       "${SPIRV_REMAPPER_LIB}"
                         IMPORTED_LOCATION_DEBUG "${SPIRV_REMAPPER_DLIB}")

    set (SPIRV_TOOLS_LIBRARIES SPIRV-Tools-opt SPIRV-Tools)
    set (GLSLANG_LIBRARIES glslang OGLCompiler OSDependent HLSL SPIRV SPVRemapper ${SPIRV_TOOLS_LIBRARIES})
else ()
    set (SPIRV_TOOLS_LIBRARIES ${SPIRV_TOOLS_OPT_LIB} ${SPIRV_TOOLS_LIB})
    set (GLSLANG_LIBRARIES ${GLSLANG_LIB} ${OGLCompiler_LIB} ${OSDependent_LIB} ${HLSL_LIB} ${SPIRV_LIB} ${SPIRV_REMAPPER_LIB} ${SPIRV_TOOLS_LIBRARIES})
    message(STATUS  ${GLSLANG_LIBRARIES} ${GLSLANG_LIB})
endif()


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

include_directories(${XCB_INCLUDE_DIRS} "./" ${GLSLANG_SPIRV_INCLUDE_DIR})

link_libraries(${XCB_LIBRARIES} m)

add_definitions(-DVK_USE_PLATFORM_XCB_KHR)

add_executable(vulkan_tutorial_01 ${source})

target_link_libraries(vulkan_tutorial_01 ${UTILS_NAME} ${CMAKE_DL_LIBS} ${GLSLANG_LIBRARIES} pthread)
