IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/Hanabi/vendor/spdlog/include"
IncludeDir["filewatch"] = "%{wks.location}/Hanabi/vendor/filewatch"
IncludeDir["GLFW"] = "%{wks.location}/Hanabi/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Hanabi/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Hanabi/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Hanabi/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Hanabi/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Hanabi/vendor/entt"
IncludeDir["yaml_cpp"] = "%{wks.location}/Hanabi/vendor/yaml_cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Hanabi/vendor/ImGuizmo"
IncludeDir["Box2D"] = "%{wks.location}/Hanabi/vendor/box2D/include"
IncludeDir["mono"] = "%{wks.location}/Hanabi/vendor/mono/include"

LibraryDir = {}
LibraryDir["mono"] = "%{wks.location}/Hanabi/vendor/mono/lib/%{cfg.buildcfg}"

Library = {}
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

-- Windows
Library["DX11"] = "d3d11.lib"
Library["DXC"] = "D3DCompiler.lib"
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"