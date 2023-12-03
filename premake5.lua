include "Dependencies.lua"

workspace "Hanabi"
	architecture "x64"
	configurations { "Debug", "Release", "Dist" }
	startproject "Hanabi_Editor"

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{wks.location}/Binaries/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
intdir = "%{wks.location}/Binaries/Intermediates/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "Hanabi/vendor/GLFW"
	include "Hanabi/vendor/imgui"
	include "Hanabi/vendor/Box2D"
	include "Hanabi/vendor/yaml_cpp"
	include "Hanabi/vendor/msdf_atlas_gen"
	include "Hanabi/vendor/Physix2D/Physix2D"
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