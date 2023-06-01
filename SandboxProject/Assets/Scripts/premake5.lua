workspace "Sandbox"
	architecture "x86_64"
	startproject "Sandbox"

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

project "Sandbox"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("Binaries")
	objdir ("Binaries/Intermediates/")

	files 
	{
		"Source/**.cs",
		"Properties/**.cs"
	}

	links
	{
		"Hanabi_ScriptCore"
	}
	
	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"

group "ScriptCore"
	include ("../../../Hanabi_ScriptCore")
group ""