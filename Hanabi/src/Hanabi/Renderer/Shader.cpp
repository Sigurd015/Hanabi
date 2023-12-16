#include "hnbpch.h"
#include "Hanabi/Renderer/Shader.h"
#include "ShaderCompiler/ShaderCompiler.h"

namespace Hanabi
{
	Scope<ShaderCompiler> s_ShaderCompiler = nullptr;

	ShaderLibrary::ShaderLibrary()
	{
		s_ShaderCompiler = ShaderCompiler::Create();
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		HNB_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& fileName)
	{
		auto shader = s_ShaderCompiler->Compile(fileName);
		Add(fileName, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		HNB_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}