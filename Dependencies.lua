VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/Hanabi/vendor/spdlog/include"
IncludeDir["filewatch"] = "%{wks.location}/Hanabi/vendor/filewatch"
IncludeDir["GLFW"] = "%{wks.location}/Hanabi/vendor/GLFW/include"
IncludeDir["ImGui"] = "%{wks.location}/Hanabi/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Hanabi/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Hanabi/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Hanabi/vendor/entt"
IncludeDir["yaml_cpp"] = "%{wks.location}/Hanabi/vendor/yaml_cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Hanabi/vendor/ImGuizmo"
IncludeDir["Box2D"] = "%{wks.location}/Hanabi/vendor/box2D/include"
IncludeDir["mono"] = "%{wks.location}/Hanabi/vendor/mono/include"
IncludeDir["msdfgen"] = "%{wks.location}/Hanabi/vendor/msdf_atlas_gen/msdfgen"
IncludeDir["msdf_atlas_gen"] = "%{wks.location}/Hanabi/vendor/msdf_atlas_gen/msdf-atlas-gen"
IncludeDir["assimp"] = "%{wks.location}/Hanabi/vendor/assimp/include"
IncludeDir["Vulkan"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["mono_Debug"] = "%{wks.location}/Hanabi/vendor/mono/lib/Debug"
LibraryDir["mono_Release"] = "%{wks.location}/Hanabi/vendor/mono/lib/Release"
LibraryDir["assimp_Debug"] = "%{wks.location}/Hanabi/vendor/assimp/bin/Debug"
LibraryDir["assimp_Release"] = "%{wks.location}/Hanabi/vendor/assimp/bin/Release"
LibraryDir["Vulkan"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["mono_Debug"] = "%{LibraryDir.mono_Debug}/libmono-static-sgen.lib"
Library["mono_Release"] = "%{LibraryDir.mono_Release}/libmono-static-sgen.lib"
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