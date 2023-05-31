project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir (outputdir .. "/%{prj.name}")
	objdir (intdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Hanabi/src",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"Hanabi",
		"Hanabi_ScriptCore"
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