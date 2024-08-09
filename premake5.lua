-- premake5.lua
include "Dependencies.lua"

workspace "Hanabi"
	architecture "x64"
	configurations { "Debug", "Release", "Dist" }
	startproject "Hanabi_Editor"

	-- Workspace-wide build options for MSVC
	filter "system:windows"
	buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }
	
	flags { "MultiProcessorCompile" }

outputdir = "%{wks.location}/Binaries/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
intdir = "%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "Hanabi/vendor/GLFW"
	include "Hanabi/vendor/imgui"
	include "Hanabi/vendor/Box2D"
	include "Hanabi/vendor/yaml_cpp"
	include "Hanabi/vendor/msdf_atlas_gen"
group ""

group "Core"
	include "Hanabi"
	include "Hanabi_ScriptCore"
group ""

group "Tools"
	include "Hanabi_Editor"
group ""

group "Misc"
	include "Sandbox"
group ""