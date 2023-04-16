#pragma once
#include "Engine/Renderer/Shader.h"
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Hanabi
{
	class OpenGLShader :public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();
		void Bind() const override;
		void Unbind() const override;
		void SetUniform(const std::string& name, const glm::vec3& value) override;
		void SetUniform(const std::string& name, const glm::mat4& value) override;
		void SetUniform(const std::string& name, const int& value) override;
		void UploadUniformInt(const std::string& name, const int& value);
		void UploadUniformMat4(const std::string& name, const glm::mat4& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
	private:
		GLint GetUniformLocation(const std::string& name) const;
		uint32_t m_RendererID;
		mutable	std::unordered_map<std::string, GLint> m_UniformLocationCache;
	};

}