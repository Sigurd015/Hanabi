file(GLOB_RECURSE SANDBOX_SRC
    "src/*.cpp"
    "src/*.h"
)

add_executable(Sandbox ${SANDBOX_SRC})

# Automatically assign source groups
assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}" SANDBOX_SRC)

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
)

# --- Link dependencies ---
target_link_libraries(Sandbox PRIVATE
    Hanabi
)

# --- Platform-specific defines ---
if (WIN32)
    target_compile_definitions(Sandbox PRIVATE
        HNB_PLATFORM_WINDOWS
    )
endif()

# --- Build configurations ---
set_target_properties(Sandbox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${OUTPUT_DIR}/Sandbox/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/Sandbox/Release"
    RUNTIME_OUTPUT_DIRECTORY_DIST    "${OUTPUT_DIR}/Sandbox/Dist"
)

# --- Post-build commands to copy Assimp DLL ---
set(ASSIMP_DLL_DEBUG "${DLL_assimp_Debug}")
set(ASSIMP_DLL_RELEASE "${DLL_assimp_Release}")

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
