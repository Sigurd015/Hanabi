# Vulkan SDK environment variable
set(VULKAN_SDK $ENV{VULKAN_SDK})

# ===== Include Directories =====
set(IncludeDir_spdlog          "${CMAKE_SOURCE_DIR}/Hanabi/vendor/spdlog/include")
set(IncludeDir_filewatch       "${CMAKE_SOURCE_DIR}/Hanabi/vendor/filewatch")
set(IncludeDir_GLFW            "${CMAKE_SOURCE_DIR}/Hanabi/vendor/GLFW/include")
set(IncludeDir_ImGui           "${CMAKE_SOURCE_DIR}/Hanabi/vendor/imgui")
set(IncludeDir_glm             "${CMAKE_SOURCE_DIR}/Hanabi/vendor/glm")
set(IncludeDir_stb_image       "${CMAKE_SOURCE_DIR}/Hanabi/vendor/stb_image")
set(IncludeDir_entt            "${CMAKE_SOURCE_DIR}/Hanabi/vendor/entt")
set(IncludeDir_yaml_cpp        "${CMAKE_SOURCE_DIR}/Hanabi/vendor/yaml_cpp/include")
set(IncludeDir_ImGuizmo        "${CMAKE_SOURCE_DIR}/Hanabi/vendor/ImGuizmo")
set(IncludeDir_Box2D           "${CMAKE_SOURCE_DIR}/Hanabi/vendor/Box2D/include")
set(IncludeDir_mono            "${CMAKE_SOURCE_DIR}/Hanabi/vendor/mono/include")
set(IncludeDir_msdfgen         "${CMAKE_SOURCE_DIR}/Hanabi/vendor/msdf_atlas_gen/msdfgen")
set(IncludeDir_msdf_atlas_gen  "${CMAKE_SOURCE_DIR}/Hanabi/vendor/msdf_atlas_gen/msdf-atlas-gen")
set(IncludeDir_assimp          "${CMAKE_SOURCE_DIR}/Hanabi/vendor/assimp/include")
set(IncludeDir_Vulkan          "${VULKAN_SDK}/Include")

# ===== Library Directories =====
set(LibraryDir_mono_Debug      "${CMAKE_SOURCE_DIR}/Hanabi/vendor/mono/lib/Debug")
set(LibraryDir_mono_Release    "${CMAKE_SOURCE_DIR}/Hanabi/vendor/mono/lib/Release")
set(LibraryDir_assimp_Debug    "${CMAKE_SOURCE_DIR}/Hanabi/vendor/assimp/bin/Debug")
set(LibraryDir_assimp_Release  "${CMAKE_SOURCE_DIR}/Hanabi/vendor/assimp/bin/Release")
set(LibraryDir_Vulkan          "${VULKAN_SDK}/Lib")

# ===== Libraries =====
set(Library_mono_Debug         "${LibraryDir_mono_Debug}/libmono-static-sgen.lib")
set(Library_mono_Release       "${LibraryDir_mono_Release}/libmono-static-sgen.lib")
set(Library_assimp_Debug       "${LibraryDir_assimp_Debug}/assimp-vc143-mtd.lib")
set(Library_assimp_Release     "${LibraryDir_assimp_Release}/assimp-vc143-mt.lib")
set(Library_Vulkan             "${LibraryDir_Vulkan}/vulkan-1.lib")

# ===== Windows System Libraries =====
set(Library_DX11               "d3d11.lib")
set(Library_DXC                "d3dCompiler.lib")
set(Library_DXGuid             "dxguid.lib")
set(Library_WinSock            "Ws2_32.lib")
set(Library_WinMM              "Winmm.lib")
set(Library_WinVersion         "Version.lib")
set(Library_BCrypt             "Bcrypt.lib")