#include "hnbpch.h"
#include "ShaderPreprocessor.h"
#include "Hanabi/Core/FileSystem.h"

namespace Hanabi
{
	namespace Utils
	{
		static ShaderType ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return VertexShader;
			if (type == "geometry")
				return GeometryShader;
			if (type == "pixel")
				return PixelShader;
			if (type == "compute")
				return ComputeShader;

			HNB_CORE_ASSERT(false, "Unknown shader type!");
			return Unknown;
		}
	}

	std::vector<ShaderSource> ShaderPreprocessor::Preprocess(const std::string& shaderName)
	{
		std::string source = ReadShaderFile(shaderName);
		PreprocessHeader(source);
		return PreprocessShader(source);
	}

	std::string ShaderPreprocessor::ReadShaderFile(const std::string& shaderName)
	{
		std::string filePath = std::string(GetShaderDirectoryPath()) + shaderName + ".hlsl";
		std::string source = FileSystem::ReadFileText(filePath);
		return source;
	}

	void ShaderPreprocessor::PreprocessHeader(std::string& source)
	{
		const char* includeToken = "#include \"";
		const char* fileType = ".hlsl\"";
		size_t includeTokenLength = strlen(includeToken);
		size_t fileTypeLength = strlen(fileType);
		size_t pos = source.find(includeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			size_t begin = pos + includeTokenLength;
			std::string fileName = source.substr(begin, eol - begin - fileTypeLength);
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			std::string filePath = std::string(GetShaderDirectoryPath()) + "include/" + fileName + ".hlsl";
			std::string includeSource = FileSystem::ReadFileText(filePath);
			includeSource = "#ifndef " + fileName + "\n" + "#define " + fileName + "\n" + includeSource + "\n" + "#endif" + "\n";
			source.replace(pos, eol - pos, includeSource);
			pos = source.find(includeToken, nextLinePos);
		}
	}

	std::vector<ShaderSource> ShaderPreprocessor::PreprocessShader(const std::string& source)
	{
		std::vector<ShaderSource> shaderSources;
		const char* typeToken = "#type:";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			size_t begin = pos + typeTokenLength; //Start of shader type name (after "#type:" keyword)
			std::string type = source.substr(begin, eol - begin);
			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line
			shaderSources.push_back({ Utils::ShaderTypeFromString(type),
				(pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos) });
		}
		return shaderSources;
	}
}