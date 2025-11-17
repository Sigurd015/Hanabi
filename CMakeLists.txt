cmake_minimum_required(VERSION 3.20)
project(Hanabi LANGUAGES C CXX CSharp)

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
add_subdirectory(Hanabi/vendor/imgui)

# ==== Core Projects ====
add_subdirectory(Hanabi)

# ==== Tools ====
add_subdirectory(Hanabi_Editor)

# ==== Misc ====
add_subdirectory(Sandbox)