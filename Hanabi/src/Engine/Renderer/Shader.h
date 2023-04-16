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
		virtual void SetValue(const std::string& name, glm::vec3 value) = 0;
		static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};
}