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

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
		uint32_t m_RendererID;
		std::string m_Name;
	};
}