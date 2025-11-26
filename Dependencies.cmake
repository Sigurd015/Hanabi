# Vulkan SDK environment variable
set(VULKAN_SDK $ENV{VULKAN_SDK})

# ===== Include Directories =====
set(IncludeDir_spdlog          "${CMAKE_SOURCE_DIR}/Hanabi/vendor/spdlog/include")
set(IncludeDir_filewatch       "${CMAKE_SOURCE_DIR}/Hanabi/vendor/filewatch")
set(IncludeDir_GLFW            "${CMAKE_SOURCE_DIR}/Hanabi/vendor/GLFW/include")
set(IncludeDir_ImGui           "${CMAKE_SOURCE_DIR}/Hanabi/vendor/imgui/imgui")
set(IncludeDir_glm             "${CMAKE_SOURCE_DIR}/Hanabi/vendor/glm")
set(IncludeDir_stb_image       "${CMAKE_SOURCE_DIR}/Hanabi/vendor/stb/stb")
set(IncludeDir_entt            "${CMAKE_SOURCE_DIR}/Hanabi/vendor/entt/single_include")
set(IncludeDir_yaml_cpp        "${CMAKE_SOURCE_DIR}/Hanabi/vendor/yaml_cpp/include")
set(IncludeDir_ImGuizmo        "${CMAKE_SOURCE_DIR}/Hanabi/vendor/ImGuizmo")
set(IncludeDir_Box2D           "${CMAKE_SOURCE_DIR}/Hanabi/vendor/Box2D/include")
set(IncludeDir_assimp          "${CMAKE_SOURCE_DIR}/Hanabi/vendor/assimp/include")
set(IncludeDir_nvrhi           "${CMAKE_SOURCE_DIR}/Hanabi/vendor/NVRHI/include")
set(IncludeDir_Vulkan          "${VULKAN_SDK}/Include")

# ===== Library Directories =====
set(LibraryDir_assimp_Debug    "${CMAKE_SOURCE_DIR}/Prebuilt/assimp/Debug")
set(LibraryDir_assimp_Release  "${CMAKE_SOURCE_DIR}/Prebuilt/assimp/Release")
set(LibraryDir_Vulkan          "${VULKAN_SDK}/Lib")

# ===== Libraries =====
set(Library_assimp_Debug       "${LibraryDir_assimp_Debug}/assimp-vc143-mtd.lib")
set(Library_assimp_Release     "${LibraryDir_assimp_Release}/assimp-vc143-mt.lib")
set(Library_Vulkan             "${LibraryDir_Vulkan}/vulkan-1.lib")

# ===== DLL =====
set(DLL_assimp_Debug "${LibraryDir_assimp_Debug}/assimp-vc143-mtd.dll")
set(DLL_assimp_Release "${LibraryDir_assimp_Release}/assimp-vc143-mt.dll")

# ===== Windows System Libraries =====
set(Library_DX11               "d3d11.lib")
set(Library_DXC                "d3dCompiler.lib")
set(Library_DXGuid             "dxguid.lib")
set(Library_WinSock            "Ws2_32.lib")
set(Library_WinMM              "Winmm.lib")
set(Library_WinVersion         "Version.lib")
set(Library_BCrypt             "Bcrypt.lib")