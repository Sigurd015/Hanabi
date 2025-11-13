file(GLOB_RECURSE SANDBOX_SRC
    "src/*.cpp"
    "src/*.h"
)

add_executable(Sandbox ${SANDBOX_SRC})

# Automatically assign source groups
assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/Hanabi/src" HANABI_SRC)

set_target_properties(Sandbox PROPERTIES
    FOLDER "Misc"    
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# --- Include directories ---
target_include_directories(Sandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/Hanabi/src
    ${IncludeDir_ImGui}
    ${IncludeDir_spdlog}
    ${IncludeDir_glm}
    ${IncludeDir_GLFW}
    ${IncludeDir_entt}
    ${IncludeDir_msdfgen}
    ${IncludeDir_msdf_atlas_gen}
)

# --- Link dependencies ---
target_link_libraries(Sandbox PRIVATE
    Hanabi
    Hanabi_ScriptCore
)

# --- Platform-specific defines ---
if (WIN32)
    target_compile_definitions(Sandbox PRIVATE
        HNB_PLATFORM_WINDOWS
    )
endif()

# --- Build configurations ---
set_target_properties(Sandbox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/Binaries/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Binaries/Release"
    RUNTIME_OUTPUT_DIRECTORY_DIST    "${CMAKE_BINARY_DIR}/Binaries/Dist"
)

# --- Post-build commands to copy Assimp DLL ---
set(ASSIMP_DLL_DEBUG "${LibraryDir_assimp_Debug}/assimp-vc143-mtd.dll")
set(ASSIMP_DLL_RELEASE "${LibraryDir_assimp_Release}/assimp-vc143-mt.dll")

add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<$<CONFIG:Debug>:${ASSIMP_DLL_DEBUG}>
        $<$<CONFIG:Release>:${ASSIMP_DLL_RELEASE}>
        $<$<CONFIG:Dist>:${ASSIMP_DLL_RELEASE}>
        $<TARGET_FILE_DIR:Sandbox>
)

# --- Configuration-specific defines ---
target_compile_definitions(Sandbox PRIVATE
    $<$<CONFIG:Debug>:HNB_DEBUG>
    $<$<CONFIG:Release>:HNB_RELEASE>
    $<$<CONFIG:Dist>:HNB_DIST>
)
