#pragma once
#include "Asset.h"

#include <filesystem>

namespace Hanabi
{
	struct AssetMetadata
	{
		AssetHandle Handle = 0;
		AssetType Type = AssetType::None;
		std::filesystem::path FilePath;

		bool IsDataLoaded = false;
		bool IsMemoryAsset = false;

		operator bool() const { return Type != AssetType::None; }
		
		bool IsValid() const { return Handle != 0 && !IsMemoryAsset; }
	};
}