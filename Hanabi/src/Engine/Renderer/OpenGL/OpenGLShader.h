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
		OpenGLShader(const std::string& filepath);
		~OpenGLShader();
		void Bind() const override;
		void Unbind() const override;
		const std::string& GetName() const override { return m_Name; }
		void SetUniform(const std::string& name, int value) override;
		void SetUniform(const std::string& name, int* values, uint32_t count) override;
		void SetUniform(const std::string& name, float value) override;
		void SetUniform(const std::string& name, const glm::vec2& value) override;
		void SetUniform(const std::string& name, const glm::vec3& value) override;
		void SetUniform(const std::string& name, const glm::vec4& value) override;
		void SetUniform(const std::string& name, const glm::mat4& value) override;
		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformVec2(const std::string& name, const glm::vec2& value);
		void UploadUniformVec3(const std::string& name, const glm::vec3& value);
		void UploadUniformVec4(const std::string& name, const glm::vec4& value);
		void UploadUniformMat4(const std::string& name, const glm::mat4& value);

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
		GLint GetUniformLocation(const std::string& name) const;
		uint32_t m_RendererID;
		mutable	std::unordered_map<std::string, GLint> m_UniformLocationCache;
		std::string m_Name;
	};

}