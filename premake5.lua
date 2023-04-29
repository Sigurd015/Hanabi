workspace "Hanabi"
	architecture "x86_64"
	startproject "Hanabi_Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{wks.location}/build/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
intdir = "%{wks.location}/build/int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/Hanabi/vendor/spdlog/include"
IncludeDir["GLFW"] = "%{wks.location}/Hanabi/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Hanabi/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Hanabi/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Hanabi/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Hanabi/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Hanabi/vendor/entt"
IncludeDir["yaml_cpp"] = "%{wks.location}/Hanabi/vendor/yaml_cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Hanabi/vendor/ImGuizmo"
IncludeDir["Box2D"] = "%{wks.location}/Hanabi/vendor/box2D/include"

group "Dependencies"
	include "Hanabi/vendor/GLFW"
	include "Hanabi/vendor/Glad"
	include "Hanabi/vendor/imgui"
	include "Hanabi/vendor/Box2D"
	include "Hanabi/vendor/yaml_cpp"

group ""

project "Hanabi"
	location "Hanabi"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir (outputdir .. "/%{prj.name}")
	objdir (intdir .. "/%{prj.name}")

	pchheader "hnbpch.h"
	pchsource "Hanabi/src/hnbpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.ImGuizmo}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"yaml_cpp",
		"Box2D",
		"opengl32.lib"
	}
	
	filter "system:windows"
		systemversion "latest"
		
		defines
		{
			"HNB_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "HNB_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HNB_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HNB_DIST"
		runtime "Release"
		optimize "on"

project "Hanabi_Editor"
	location "Hanabi_Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir (outputdir .. "/%{prj.name}")
	objdir (intdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"Hanabi/src",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}"
	}
	
	links
	{
		"Hanabi"
	}
	
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"HNB_PLATFORM_WINDOWS"
		}
	
	filter "configurations:Debug"
		defines "HNB_DEBUG"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "HNB_RELEASE"
		runtime "Release"
		optimize "on"
	
	filter "configurations:Dist"
		defines "HNB_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir (outputdir .. "/%{prj.name}")
	objdir (intdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Hanabi/src",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"Hanabi"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"HNB_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "HNB_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HNB_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HNB_DIST"
		runtime "Release"
		optimize "on"