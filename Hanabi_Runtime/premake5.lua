project "Hanabi_Runtime"
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
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.msdfgen}",
		"%{IncludeDir.msdf_atlas_gen}"
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
	    kind "ConsoleApp"
		defines "HNB_DEBUG"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
	    kind "ConsoleApp"
		defines "HNB_RELEASE"
		runtime "Release"
		optimize "on"
	
	filter "configurations:Dist"
	    kind "WindowedApp"
		defines "HNB_DIST"
		runtime "Release"
		optimize "on"