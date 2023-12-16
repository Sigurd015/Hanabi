#pragma once
#include "ShaderCompiler/ShaderCompiler.h"

#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace Hanabi
{
	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void Bind() const = 0;
		virtual const std::vector<ShaderResourceDeclaration>& GetReflectionData() const = 0;
	};

	//TODO:This should be handled by the Asset Manager
	class ShaderLibrary
	{
	public:
		ShaderLibrary();
		void Add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& fileName);
		Ref<Shader> Get(const std::string& name);
		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}