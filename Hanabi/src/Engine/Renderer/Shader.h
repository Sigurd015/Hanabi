#pragma once

#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace Hanabi
{
	using ShaderReflectionData = std::unordered_map<std::string, uint32_t>;

	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual const std::string& GetName() const = 0;
		virtual const ShaderReflectionData& GetReflectionData() const = 0;

		static Ref<Shader> Create(const std::string& fileName);

		static constexpr const char* GetShaderDirectoryPath()
		{
			return "resources/shaders/";
		}
	};

	//TODO:This should be handled by the Asset Manager
	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& fileName);
		Ref<Shader> Load(const std::string& name, const std::string& fileName);
		Ref<Shader> Get(const std::string& name);
		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}