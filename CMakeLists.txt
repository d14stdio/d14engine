#################
# Project Setup #
#################

# Ninja Multi-Config requires CMake 3.17 or later.
cmake_minimum_required(VERSION 3.17)

project(D14Engine VERSION 0.1.0)
message(STATUS "D14Engine v${PROJECT_VERSION}")

# It is not clear why this does not work on Windows.
# We have to set -G"Ninja Multi-Config" manually in the args.
# set(CMAKE_GENERATOR "Ninja Multi-Config")

####################
# Compile Settings #
####################

# This can help analyze the actual build process.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#----------------------------------------------------------
# C++ Standard
#----------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#----------------------------------------------------------
# Compile Definitions
#----------------------------------------------------------

add_compile_definitions(
    _CONSOLE _UNICODE NOMINMAX UNICODE
)
#----------------------------------------------------------
# Include Directories
#----------------------------------------------------------

include_directories(
    "include" "include/DirectX" "src"
)
#----------------------------------------------------------
# Compiler Options
#----------------------------------------------------------

# Note that cl.exe does not set /utf-8 by default.
add_compile_options(/utf-8)

##################
# Target Configs #
##################

#----------------------------------------------------------
# Functions used to help locate target source files.
#----------------------------------------------------------

# Get a global property: D14Target_<name>=<path>.
function(GetTargetPath name path)
    get_property(val GLOBAL PROPERTY D14Target_${name})
    set(${path} ${val} PARENT_SCOPE)
endfunction()

# Set a global property: D14Target_<name>=<path>.
function(SetTargetPath name path)
    set_property(GLOBAL PROPERTY D14Target_${name} ${path})
endfunction()

# Check if a target named D14Target_<name> exists.
function(IsTargetValid name result)
    get_property(val GLOBAL PROPERTY D14Target_${name} SET)
    set(${result} ${val} PARENT_SCOPE)
endfunction()

#----------------------------------------------------------
# Get source files of each submodule and build target.
#----------------------------------------------------------

file(GLOB_RECURSE srcCommon     "src/Common/*.cpp")
file(GLOB_RECURSE srcEditor     "src/Editor/*.cpp")
file(GLOB_RECURSE srcPipeline   "src/Pipeline/*.cpp")
file(GLOB_RECURSE srcRenderer   "src/Renderer/*.cpp")
file(GLOB_RECURSE srcUIKit      "src/UIKit/*.cpp")

SetTargetPath("SimpleWindow"    "test/Renderer/SimpleWindow")

SetTargetPath("ButtonFamily"    "test/UIKit/ButtonFamily")
SetTargetPath("ColorfulCube"    "test/UIKit/ColorfulCube")
SetTargetPath("DemoTemplate"    "test/UIKit/DemoTemplate")
SetTargetPath("ImageViewer"     "test/UIKit/ImageViewer")
SetTargetPath("ListTreeView"    "test/UIKit/ListTreeView")
SetTargetPath("PixelAnimation"  "test/UIKit/PixelAnimation")
SetTargetPath("SimpleEditor"    "test/UIKit/SimpleEditor")
SetTargetPath("VariousFonts"    "test/UIKit/VariousFonts")
SetTargetPath("WidgetsGallery"  "test/UIKit/WidgetsGallery")

SetTargetPath("EmptyScene"      "test/Pipeline/EmptyScene")
SetTargetPath("ManySprites"     "test/Pipeline/2D/ManySprites")

#----------------------------------------------------------
# Decide source files involved in target generation.
#----------------------------------------------------------

if(NOT DEFINED D14_TARGET)
    set(D14_TARGET "SimpleWindow")
endif()
# Try to get the source path of the specified target.
IsTargetValid(${D14_TARGET} D14_TARGET_VALID)
if (NOT D14_TARGET_VALID)
    message(FATAL_ERROR "Target ${D14_TARGET} is not valid.")
endif()
GetTargetPath(${D14_TARGET} D14_TARGET_PATH)

message(STATUS "Target name: ${D14_TARGET}")
message(STATUS "Target path: ${D14_TARGET_PATH}")

set(D14_SOURCE ${srcCommon})

if (${D14_TARGET_PATH} MATCHES "test/Renderer/*")
    list(APPEND D14_SOURCE ${srcRenderer})
endif()
if (${D14_TARGET_PATH} MATCHES "test/UIKit/*")
    list(APPEND D14_SOURCE ${srcRenderer})
    list(APPEND D14_SOURCE ${srcUIKit})
endif()
if (${D14_TARGET_PATH} MATCHES "test/Pipeline/*")
    list(APPEND D14_SOURCE ${srcRenderer})
    list(APPEND D14_SOURCE ${srcUIKit})
    list(APPEND D14_SOURCE ${srcPipeline})
endif()

file(GLOB_RECURSE srcD14Target "${D14_TARGET_PATH}/*.cpp")
list(APPEND D14_SOURCE ${srcD14Target})

#----------------------------------------------------------
# Add the final build target and related commands.
#----------------------------------------------------------

add_executable(${D14_TARGET} ${D14_SOURCE})

target_precompile_headers(
    ${D14_TARGET} PRIVATE
    "src/Common/Precompile.h"
)
target_link_options(
    ${D14_TARGET} PRIVATE
    "/DELAYLOAD:dxcompiler.dll"
)
# Since some demo programs require to load DLLs, shaders,
# or resources based on relative paths, D14Target must be
# copied to the project root directory before debugging.
add_custom_command(
    TARGET ${D14_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:${D14_TARGET}>"
    "${PROJECT_SOURCE_DIR}/d14engine.exe"
)
