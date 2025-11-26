# === Source files ===
file(GLOB_RECURSE HANABI_SRC
    "src/*.cpp"
    "src/*.h"
    "vendor/glm/glm/*.hpp"
    "vendor/glm/glm/*.inl"
	"vendor/stb/stb/stb_image.h"
	"vendor/stb/stb_image.cpp"
    "vendor/ImGuizmo/ImGuizmo.h"
    "vendor/ImGuizmo/ImGuizmo.cpp"
)

# === Basic setup ===
add_library(Hanabi STATIC)

# Automatically assign source groups
assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}" HANABI_SRC)

set_target_properties(Hanabi PROPERTIES
    FOLDER "Core"    
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_sources(Hanabi PRIVATE ${HANABI_SRC})

# === Output directories ===
set_target_properties(Hanabi PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/Hanabi"
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/Hanabi"
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/Hanabi"
)

# === Precompiled header ===
target_precompile_headers(Hanabi PRIVATE "src/hnbpch.h")

# === Preprocessor defines ===
target_compile_definitions(Hanabi PRIVATE
    _CRT_SECURE_NO_WARNINGS
    GLFW_INCLUDE_NONE
    YAML_CPP_STATIC_DEFINE
    GLM_FORCE_DEPTH_ZERO_TO_ONE
)

# === Include directories ===
target_include_directories(Hanabi PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${IncludeDir_spdlog}
    ${IncludeDir_filewatch}
    ${IncludeDir_GLFW}
    ${IncludeDir_ImGui}
    ${IncludeDir_glm}
    ${IncludeDir_stb_image}
    ${IncludeDir_entt}
    ${IncludeDir_yaml_cpp}
    ${IncludeDir_Box2D}
    ${IncludeDir_ImGuizmo}
    ${IncludeDir_assimp}
    ${IncludeDir_Vulkan}
    ${IncludeDir_nvrhi}
)

# === Link libraries ===
target_link_libraries(Hanabi 
    PUBLIC
        glfw
        ImGui
        yaml-cpp
        box2d
    PRIVATE
        nvrhi
        nvrhi_vk    
)

# === Disable PCH for certain vendor files ===
# CMake doesn’t have “filter NoPCH” per file, but you can mark sources manually:
set_source_files_properties(
    vendor/stb/stb_image.cpp
    vendor/ImGuizmo/ImGuizmo.cpp
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)

# === Platform-specific ===
if(WIN32)
    target_compile_definitions(Hanabi PRIVATE
        HNB_PLATFORM_WINDOWS
        HNB_ENABLE_D3D
        HNB_ENABLE_VULKAN
        GLFW_EXPOSE_NATIVE_WIN32
        GLFW_NATIVE_INCLUDE_NONE
    )

    target_link_libraries(Hanabi PRIVATE
        ${Library_DX11}
        ${Library_DXC}
        ${Library_DXGuid}
        ${Library_WinSock}
        ${Library_WinMM}
        ${Library_WinVersion}
        ${Library_BCrypt}
        ${Library_Vulkan}
        nvrhi_d3d12
    )
endif()

# === Configuration-specific ===
# These mirror Premake’s Debug/Release/Dist filters
target_compile_definitions(Hanabi PRIVATE
    $<$<CONFIG:Debug>:HNB_DEBUG>
    $<$<CONFIG:Release>:HNB_RELEASE>
    $<$<CONFIG:Dist>:HNB_DIST>
)

# Runtime library type (equivalent to staticruntime "off")
if(MSVC)
    # /MD for Release/Dist, /MDd for Debug
    target_compile_options(Hanabi PRIVATE
        $<$<CONFIG:Debug>:/MDd>
        $<$<CONFIG:Release>:/MD>
        $<$<CONFIG:Dist>:/MD>
    )
endif()

# Debug / Release / Dist options
target_link_libraries(Hanabi PRIVATE
    $<$<CONFIG:Debug>:${Library_assimp_Debug}>
    $<$<CONFIG:Release>:${Library_assimp_Release}>
    $<$<CONFIG:Dist>:${Library_assimp_Release}>
)

# Optimization and symbols
target_compile_options(Hanabi PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O2>
)
