#include "hnbpch.h"
#include "ShaderPreprocessor.h"
#include "Hanabi/Core/FileSystem.h"
#include "Hanabi/Utils/StringUtils.h"

namespace Hanabi
{
	namespace Utils
	{
		static ShaderType ShaderTypeFromString(const std::string_view stage)
		{
			if (stage == "vertex")
				return VertexShader;
			if (stage == "geometry")
				return GeometryShader;
			if (stage == "pixel")
				return PixelShader;
			if (stage == "compute")
				return ComputeShader;

			HNB_CORE_ASSERT(false, "Unknown shader type!");
			return Unknown;
		}

		void ContainsHeaderGuard(IncludeData& header)
		{
			size_t pos = header.IncludedFileSource.find('#');
			while (pos != std::string::npos)
			{
				const size_t endOfLine = header.IncludedFileSource.find_first_of("\r\n", pos) + 1;
				auto tokens = Utils::String::SplitStringAndKeepDelims(header.IncludedFileSource.substr(pos, endOfLine - pos));

				uint32_t index = 0;
				if (tokens[index] == "#")
				{
					++index;
					if (tokens[index] == "pragma")
					{
						++index;
						if (tokens[index] == "once")
						{
							header.IncludedFileSource.erase(pos, endOfLine - pos);
							// Get the file name without the extension
							std::string fileName = header.IncludedFilePath.stem().string();
							header.IncludedFileSource.insert(pos, "#ifndef " + fileName + "\n" + "#define " + fileName + "\n");
							header.IncludedFileSource.append("\n#endif");
						}
					}
				}
				pos = header.IncludedFileSource.find('#', pos + 1);
			}
		}
	}

	std::vector<ShaderSource> ShaderPreprocessor::Preprocess(const std::string& shaderName)
	{
		std::filesystem::path filePath = std::string(GetShaderDirectoryPath()) + shaderName + std::string(GetShaderExtension());
		std::string source = FileSystem::ReadFileText(filePath);
		PreprocessHeader(source, GetShaderDirectoryPath());
		return PreprocessShader(source);
	}

	void ShaderPreprocessor::PreprocessHeader(std::string& source, const std::filesystem::path& filePath)
	{
		const char* includeToken = "#include \"";
		size_t includeTokenLength = strlen(includeToken);

		size_t pos = source.find(includeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			size_t begin = pos + includeTokenLength;
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);

			IncludeData header;
			header.IncludedFilePath = filePath;
			// -1 to remove the last '/'
			header.IncludedFilePath /= source.substr(begin, eol - begin - 1);
			header.IncludedFileSource = FileSystem::ReadFileText(header.IncludedFilePath);
			Utils::ContainsHeaderGuard(header);
			PreprocessHeader(header.IncludedFileSource, header.IncludedFilePath.remove_filename());

			source.replace(pos, eol - pos, header.IncludedFileSource);
			pos = source.find(includeToken, nextLinePos);
		}
	}

	std::vector<ShaderSource> ShaderPreprocessor::PreprocessShader(const std::string& source)
	{
		const char* pragmaToken = "#pragma ";
		size_t pragmaTokenLength = strlen(pragmaToken);

		std::vector<ShaderSource> shaderSources;

		size_t pos = source.find(pragmaToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			size_t begin = pos + pragmaTokenLength;
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);

			auto tokens = Utils::String::SplitStringAndKeepDelims(source.substr(begin, eol - begin));

			uint32_t index = 0;

			if (tokens[index] == "stage")
			{
				++index;
				HNB_CORE_ASSERT(tokens[index] == ":", "Stage pragma is invalid");

				const std::string_view stage(tokens[++index]);
				HNB_CORE_ASSERT(stage == "vertex" || stage == "geometry" || stage == "pixel" || stage == "compute", "Invalid shader type specified");

				pos = source.find(pragmaToken, nextLinePos);
				shaderSources.push_back({ Utils::ShaderTypeFromString(stage),
					(pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos) });
			}
		}
		return shaderSources;
	}
}