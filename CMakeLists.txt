cmake_minimum_required(VERSION 3.20)
project(Hanabi LANGUAGES C CXX)

# === Dependencies.cmake ===
include(${CMAKE_SOURCE_DIR}/Dependencies.cmake)

# ==== Global Configuration ====
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "" FORCE)

# Output directories (match Premake)
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/Binaries/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
set(INT_DIR ${CMAKE_SOURCE_DIR}/Binaries/Intermediates/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

# MSVC-specific options
if(MSVC)
    add_compile_options(/EHsc /Zc:preprocessor /Zc:__cplusplus)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# Enable parallel builds
if(MSVC)
    add_compile_options(/MP)
endif()

# Enables folder grouping in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

# === Function to automatically create source groups ===
function(assign_source_groups BASE_DIR FILES_LIST)
    foreach(file_path IN LISTS ${FILES_LIST})
        # Make path relative to base directory
        file(RELATIVE_PATH rel_path "${BASE_DIR}" "${file_path}")
        get_filename_component(folder_path "${rel_path}" PATH)
        # Convert to Windows-style backslashes for VS
        string(REPLACE "/" "\\" folder_path "${folder_path}")
        source_group("${folder_path}" FILES "${file_path}")
    endforeach()
endfunction()

# ==== Dependencies ====
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL  "" )
add_subdirectory(Hanabi/vendor/GLFW)
set_property(TARGET glfw PROPERTY FOLDER "Dependencies")

if(TARGET update_mappings)
    set_property(TARGET update_mappings PROPERTY FOLDER "Dependencies/Unused")
endif()

add_subdirectory(Hanabi/vendor/imgui)

add_subdirectory(Hanabi/vendor/Box2D)
set_property(TARGET box2d PROPERTY FOLDER "Dependencies")

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "")
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "")
set(YAML_CPP_INSTALL OFF CACHE BOOL "")
add_subdirectory(Hanabi/vendor/yaml_cpp)
set_property(TARGET yaml-cpp PROPERTY FOLDER "Dependencies")

set(NVRHI_INSTALL OFF CACHE BOOL "")
set(NVRHI_INSTALL_EXPORTS OFF CACHE BOOL "")
set(NVRHI_WITH_VALIDATION OFF CACHE BOOL "")
set(NVRHI_WITH_RTXMU OFF CACHE BOOL "")
set(NVRHI_WITH_AFTERMATH OFF CACHE BOOL "")
set(NVRHI_WITH_NVAPI OFF CACHE BOOL "")
set(NVRHI_WITH_DX11 OFF CACHE BOOL "")
set(NVRHI_WITH_VULKAN ON CACHE BOOL "")
if(WIN32)
    set(NVRHI_WITH_DX12 ON CACHE BOOL "")
endif()
if(UNIX AND NOT APPLE)
    set(NVRHI_WITH_DX12 OFF CACHE BOOL "")
endif()

add_subdirectory(Hanabi/vendor/NVRHI)

set_property(TARGET nvrhi PROPERTY FOLDER "Dependencies/NVRHI")
set_property(TARGET nvrhi_d3d12 PROPERTY FOLDER "Dependencies/NVRHI")
set_property(TARGET nvrhi_vk PROPERTY FOLDER "Dependencies/NVRHI")
set_property(TARGET DirectX-Guids PROPERTY FOLDER "Dependencies/NVRHI")
set_property(TARGET DirectX-Headers PROPERTY FOLDER "Dependencies/NVRHI")

# ==== Core Projects ====
add_subdirectory(Hanabi)

# ==== Tools ====
add_subdirectory(Hanabi_Editor)

# ==== Misc ====
add_subdirectory(Sandbox)