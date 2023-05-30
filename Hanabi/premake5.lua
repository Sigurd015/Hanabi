project "Hanabi"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir (outputdir .. "/%{prj.name}")
	objdir (intdir .. "/%{prj.name}")

	pchheader "hnbpch.h"
	pchsource "src/hnbpch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.mono}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"yaml_cpp",
		"Box2D",
		"opengl32.lib",
		"%{Library.mono}"
	}
	
	filter "system:windows"
		systemversion "latest"
		
		links
		{
			"%{Library.DX11}",
			"%{Library.DXC}",
			"%{Library.WinSock}",
			"%{Library.WinMM}",
			"%{Library.WinVersion}",
			"%{Library.BCrypt}"
		}
		
		defines
		{
			"HNB_PLATFORM_WINDOWS",
			"GLFW_EXPOSE_NATIVE_WIN32",
			"GLFW_NATIVE_INCLUDE_NONE"
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