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
		"YAML_CPP_STATIC_DEFINE",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.mono}",
		"%{IncludeDir.msdfgen}",
		"%{IncludeDir.msdf_atlas_gen}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.Vulkan}"
	}

	links 
	{ 
		"GLFW",
		"ImGui",
		"yaml_cpp",
		"Box2D",
		"msdf_atlas_gen",
	}

	filter "files:vendor/stb_image/**.cpp or vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }
	
	filter "system:windows"
		systemversion "latest"
		
		links
		{
			"%{Library.DX11}",
			"%{Library.DXC}",
			"%{Library.DXGuid}",
			"%{Library.WinSock}",
			"%{Library.WinMM}",
			"%{Library.WinVersion}",
			"%{Library.BCrypt}",
			"%{Library.Vulkan}"
		}
		
		defines
		{
			"HNB_PLATFORM_WINDOWS",
			"HNB_ENABLE_D3D",
			"HNB_ENABLE_VULKAN",
			"GLFW_EXPOSE_NATIVE_WIN32",
			"GLFW_NATIVE_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "HNB_DEBUG"
		runtime "Debug"
		symbols "on"

		links 
		{ 
			"%{Library.mono_Debug}",
			"%{Library.assimp_Debug}"
		}

	filter "configurations:Release"
		defines "HNB_RELEASE"
		runtime "Release"
		optimize "on"

		links 
		{ 
			"%{Library.mono_Release}",
			"%{Library.assimp_Release}"
		}

	filter "configurations:Dist"
		defines "HNB_DIST"
		runtime "Release"
		optimize "on"
		
		links 
		{ 
			"%{Library.mono_Release}",
			"%{Library.assimp_Release}"
		}