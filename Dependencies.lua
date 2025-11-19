VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/Hanabi/vendor/spdlog/include"
IncludeDir["filewatch"] = "%{wks.location}/Hanabi/vendor/filewatch"
IncludeDir["GLFW"] = "%{wks.location}/Hanabi/vendor/GLFW/include"
IncludeDir["ImGui"] = "%{wks.location}/Hanabi/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Hanabi/vendor/glm"
IncludeDir["stb"] = "%{wks.location}/Hanabi/vendor/stb/stb"
IncludeDir["entt"] = "%{wks.location}/Hanabi/vendor/entt/single_include/entt"
IncludeDir["yaml_cpp"] = "%{wks.location}/Hanabi/vendor/yaml_cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Hanabi/vendor/ImGuizmo"
IncludeDir["Box2D"] = "%{wks.location}/Hanabi/vendor/box2D/include"
IncludeDir["mono"] = "%{wks.location}/Hanabi/vendor/mono/include"
IncludeDir["assimp"] = "%{wks.location}/Hanabi/vendor/assimp/include"
IncludeDir["Vulkan"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["assimp_Debug"] = "%{wks.location}/Prebuilt/assimp/bin/Debug"
LibraryDir["assimp_Release"] = "%{wks.location}/Prebuilt/assimp/bin/Release"
LibraryDir["Vulkan"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["assimp_Debug"] = "%{LibraryDir.assimp_Debug}/assimp-vc143-mtd.lib"
Library["assimp_Release"] = "%{LibraryDir.assimp_Release}/assimp-vc143-mt.lib"
Library["Vulkan"] = "%{LibraryDir.Vulkan}/vulkan-1.lib"

-- Windows
Library["DX11"] = "d3d11.lib"
Library["DXC"] = "d3dCompiler.lib"
Library["DXGuid"] = "dxguid.lib"
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"