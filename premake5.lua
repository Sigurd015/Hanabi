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
IncludeDir["spdlog"] = "Hanabi/vendor/spdlog/include"
IncludeDir["GLFW"] = "Hanabi/vendor/GLFW/include"
IncludeDir["Glad"] = "Hanabi/vendor/Glad/include"
IncludeDir["ImGui"] = "Hanabi/vendor/imgui"
IncludeDir["glm"] = "Hanabi/vendor/glm"
IncludeDir["stb_image"] = "Hanabi/vendor/stb_image"
IncludeDir["entt"] = "Hanabi/vendor/entt"

group "Dependencies"
	include "Hanabi/vendor/GLFW"
	include "Hanabi/vendor/Glad"
	include "Hanabi/vendor/imgui"
	
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
		"%{prj.name}/vendor/stb_image/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
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
		"%{IncludeDir.entt}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
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