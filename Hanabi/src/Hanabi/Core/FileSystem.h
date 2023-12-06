#pragma once
#include "Buffer.h"

#include <filesystem>

namespace Hanabi
{
	class FileSystem
	{
	public:
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	};
}