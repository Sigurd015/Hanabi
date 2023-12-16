#include "hnbpch.h"
#include "FileSystem.h"

namespace Hanabi
{
	Buffer FileSystem::ReadFileBinary(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			// Failed to open the file
			return {};
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint64_t size = end - stream.tellg();

		if (size == 0)
		{
			// File is empty
			return {};
		}

		Buffer buffer(size);
		stream.read(buffer.As<char>(), size);
		stream.close();
		return buffer;
	}

	std::string FileSystem::ReadFileText(const std::filesystem::path& filepath)
	{
		std::string result;
		std::ifstream in(filepath.string(), std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
				in.close();
			}
			else
			{
				HNB_CORE_ERROR("Could not read from file '{0}'", filepath.string());
			}
		}
		else
		{
			HNB_CORE_ERROR("Could not open file '{0}'", filepath.string());
		}
		return result;
	}
}