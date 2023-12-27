#pragma once
#include "Hanabi/Renderer/Shader.h"

#include <string>
#include <vector>

namespace Hanabi
{
	struct IncludeData
	{
		std::filesystem::path IncludedFilePath;
		std::string IncludedFileSource;
		uint32_t IncludeOffset = 0;
		uint32_t IncludeLength = 0;

		inline bool operator==(const IncludeData& other) const noexcept
		{
			return this->IncludedFilePath == other.IncludedFilePath;
		}
	};

	struct ShaderSource
	{
		ShaderType Type;
		std::string Source;
	};

	class ShaderPreprocessor
	{
	public:
		static std::vector<ShaderSource> Preprocess(const std::string& shaderName);

		static void PreprocessHeader(std::string& source, const std::filesystem::path& filePath);
		static std::vector<ShaderSource> PreprocessShader(const std::string& source);

		static constexpr const char* GetShaderDirectoryPath()
		{
			return "Resources/Shaders/";
		}

		static constexpr const char* GetShaderExtension()
		{
			return ".hlsl";
		}
	};
}