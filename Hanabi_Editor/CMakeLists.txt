file(GLOB_RECURSE EDITOR_SRC
    "src/*.cpp"
    "src/*.h"
)

add_executable(Hanabi_Editor ${EDITOR_SRC})

# Automatically assign source groups
assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}" EDITOR_SRC)

set_target_properties(Hanabi_Editor PROPERTIES
    FOLDER "Tools"    
    CXX_STANDARD 17
)

# --- Include directories ---
target_include_directories(Hanabi_Editor PRIVATE
    src
    ${CMAKE_SOURCE_DIR}/Hanabi/src
    ${IncludeDir_ImGui}
    ${IncludeDir_spdlog}
    ${IncludeDir_filewatch}
    ${IncludeDir_glm}
    ${IncludeDir_entt}
    ${IncludeDir_GLFW}
    ${IncludeDir_ImGuizmo}
    ${IncludeDir_msdfgen}
    ${IncludeDir_msdf_atlas_gen}
)

# --- Link dependencies ---
target_link_libraries(Hanabi_Editor PRIVATE
    Hanabi
)

# --- Platform-specific options ---
if (WIN32)
    target_compile_definitions(Hanabi_Editor PRIVATE
        HNB_PLATFORM_WINDOWS
    )
endif()

# --- Build configurations ---
set_target_properties(Hanabi_Editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/Binaries/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Binaries/Release"
    RUNTIME_OUTPUT_DIRECTORY_DIST    "${CMAKE_BINARY_DIR}/Binaries/Dist"
)

# Define custom configs if not already defined
if (NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "" FORCE)
endif()

# --- Per-configuration settings ---
# Debug
set(ASSIMP_DLL_DEBUG "${LibraryDir_assimp_Debug}/assimp-vc143-mtd.dll")
set(ASSIMP_DLL_RELEASE "${LibraryDir_assimp_Release}/assimp-vc143-mt.dll")

add_custom_command(TARGET Hanabi_Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<$<CONFIG:Debug>:${ASSIMP_DLL_DEBUG}>
        $<$<CONFIG:Release>:${ASSIMP_DLL_RELEASE}>
        $<$<CONFIG:Dist>:${ASSIMP_DLL_RELEASE}>
        $<TARGET_FILE_DIR:Hanabi_Editor>
)

# --- Configuration-specific defines and options ---
target_compile_definitions(Hanabi_Editor PRIVATE
    $<$<CONFIG:Debug>:HNB_DEBUG>
    $<$<CONFIG:Release>:HNB_RELEASE>
    $<$<CONFIG:Dist>:HNB_DIST>
)