#include "Hanabi.h"
#include "EditorLayer.h"
#include "Engine/EntryPoint.h"
#include "EditorResources.h"

namespace Hanabi
{
	class HanabiEditor : public Application
	{
	public:
		HanabiEditor(const Hanabi::ApplicationSpecification& specification) : Hanabi::Application(specification)
		{
			EditorResources::Init();

			PushLayer(new EditorLayer());
		}

		~HanabiEditor()
		{
			
		}
	};
}

Hanabi::Application* Hanabi::CreateApplication(ApplicationCommandLineArgs args)
{
	Hanabi::ApplicationSpecification spec;
	spec.Name = "Hanabi Editor";
	spec.WorkingDirectory = "../Hanabi_Editor";
	spec.CommandLineArgs = args;
	spec.EnableImGui = true;
	spec.EnableScripting = true;
	spec.ScriptConfig.CoreAssemblyPath = "Resources/Scripts/Hanabi_ScriptCore.dll";
	// TODO: When we have spirv compiler, set this
	//spec.RendererConfig.ShaderPackPath = "Resources/Shaders";
	if (spec.CommandLineArgs.Count > 2)
	{
		spec.RendererConfig.APIType = static_cast<RendererAPIType>(std::stoi(spec.CommandLineArgs[2]));
	}
	else
	{
		spec.RendererConfig.APIType = RendererAPIType::DX11;
	}
	return new HanabiEditor(spec);
}