#pragma once
#include "Hanabi/Renderer/Shader.h"

#include <string>
#include <vector>

namespace Hanabi
{
	struct ShaderSource
	{
		ShaderType Type;
		std::string Source;
	};

	class ShaderPreprocessor
	{
	public:
		static std::vector<ShaderSource> Preprocess(const std::string& shaderName);

		static std::string ReadShaderFile(const std::string& shaderName);
		static void PreprocessHeader(std::string& source);
		static std::vector<ShaderSource> PreprocessShader(const std::string& source);

		static constexpr const char* GetShaderDirectoryPath()
		{
			return "Resources/Shaders/";
		}
	};
}