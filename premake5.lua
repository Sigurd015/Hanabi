include "Dependencies.lua"

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

group "Dependencies"
	include "Hanabi/vendor/GLFW"
	include "Hanabi/vendor/Glad"
	include "Hanabi/vendor/imgui"
	include "Hanabi/vendor/Box2D"
	include "Hanabi/vendor/yaml_cpp"

group ""

include "Hanabi/"
include "Hanabi_Editor/"
include "Sandbox/"