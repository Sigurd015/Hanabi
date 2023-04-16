#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Hanabi
{
	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void SetUniform(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetUniform(const std::string& name, const glm::mat4& value) = 0;
		virtual void SetUniform(const std::string& name, const int& value) = 0;
		static Ref<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};
}