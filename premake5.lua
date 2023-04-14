workspace "Hanabi"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{wks.location}/build/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
intdir = "%{wks.location}/build/int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["spdlog"] = "Hanabi/vendor/spdlog/include"
IncludeDir["GLFW"] = "Hanabi/vendor/GLFW/include"
IncludeDir["Glad"] = "Hanabi/vendor/Glad/include"
IncludeDir["ImGui"] = "Hanabi/vendor/imgui"
IncludeDir["glm"] = "Hanabi/vendor/glm"

group "Dependencies"
	include "Hanabi/vendor/GLFW"
	include "Hanabi/vendor/Glad"
	include "Hanabi/vendor/imgui"

group ""

project "Hanabi"
	location "Hanabi"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir (outputdir .. "/%{prj.name}")
	objdir (intdir .. "/%{prj.name}")

	pchheader "hnbpch.h"
	pchsource "Hanabi/src/hnbpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"opengl32.lib",
		"ImGui"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"HNB_PLATFORM_WINDOWS",
			"HNB_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "HNB_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "HNB_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "HNB_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

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
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Hanabi"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"HNB_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "HNB_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "HNB_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "HNB_DIST"
		runtime "Release"
		optimize "On"