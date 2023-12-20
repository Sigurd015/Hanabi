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

	Ref<Shader> ShaderLibrary::Load(const std::string& name)
	{
		auto shader = s_ShaderCompiler->Compile(name);
		Add(name, shader);
		return shader;
	}

	void ShaderLibrary::Reload(const std::string& name)
	{
		Ref<Shader> shader = Get(name);
		s_ShaderCompiler->TryReCompile(shader, name);
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		HNB_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	std::vector<std::string> ShaderLibrary::GetShaderNames() const
	{
		std::vector<std::string> names;
		names.reserve(m_Shaders.size());
		for (auto& pair : m_Shaders)
			names.push_back(pair.first);

		return names;
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}