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

####################
# Helper Functions #
####################

#----------------------------------------------------------
# Target List
#----------------------------------------------------------

set(TargetList "")

#----------------------------------------------------------
# Global Property
#----------------------------------------------------------

# Check if a property named <key> exists.
function(IsPropertyValid key value)
    get_property(result GLOBAL PROPERTY ${key} SET)
    set(${value} ${result} PARENT_SCOPE)
endfunction()

# Get a global property: <key>=<value>.
function(GetPropertyValue key value)
    get_property(result GLOBAL PROPERTY ${key})
    set(${value} "${result}" PARENT_SCOPE)
endfunction()

# Set a global property: <key>=<ARGN>.
function(SetPropertyValue key)
    set_property(GLOBAL PROPERTY ${key} ${ARGN})
endfunction()

###################
# Module & Target #
###################

#----------------------------------------------------------
# Find source files of each module and target.
#----------------------------------------------------------

add_subdirectory(src)
add_subdirectory(test)

#----------------------------------------------------------
# Extract source files from related properties.
#----------------------------------------------------------

GetPropertyValue(D14_CommonSource   CommonSource)
GetPropertyValue(D14_RendererSource RendererSource)
GetPropertyValue(D14_UIKitSource    UIKitSource)
GetPropertyValue(D14_PipelineSource PipelineSource)
GetPropertyValue(D14_EditorSource   EditorSource)

# Set -DTargetName=<what_you_want_to_build> in the command line
# to set a specific target, otherwise it will default to SimpleWindow.

if(NOT DEFINED TargetName)
    set(TargetName "SimpleWindow")
endif()
IsPropertyValid(D14_${TargetName}Source TargetValid)
if (TargetValid)
    message(STATUS "Set target to: ${TargetName}")
else()
    message(FATAL_ERROR "Target is not valid: ${TargetName}")
endif()

GetPropertyValue(D14_${TargetName}Module TargetModule)
GetPropertyValue(D14_${TargetName}Source TargetSource)

#----------------------------------------------------------
# Add Common module and precompile header.
#----------------------------------------------------------

set(PCH "src/Common/Precompile.h")

add_library(Common STATIC ${CommonSource})
target_precompile_headers(Common PRIVATE "${PCH}")

#----------------------------------------------------------
# Add build target and link Common module.
#----------------------------------------------------------

add_executable(${TargetName} ${TargetSource})
target_link_libraries(${TargetName} PRIVATE Common)
target_precompile_headers(${TargetName} REUSE_FROM Common)

#----------------------------------------------------------
# Add and link other modules that the target depends on.
#----------------------------------------------------------

foreach(module ${TargetModule})
    add_library(${module} STATIC ${${module}Source})
    target_link_libraries(${TargetName} PRIVATE ${module})
    target_precompile_headers(${module} REUSE_FROM Common)
endforeach()

target_link_options(${TargetName} PRIVATE "/DELAYLOAD:dxcompiler.dll")

#----------------------------------------------------------
# Copy target to root directory for easy debugging.
#----------------------------------------------------------

set(TargetFile "$<TARGET_FILE:${TargetName}>")
set(DebugExePath "${PROJECT_SOURCE_DIR}/d14engine.exe")

add_custom_command(
    TARGET ${TargetName} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${TargetFile}" "${DebugExePath}"
)
