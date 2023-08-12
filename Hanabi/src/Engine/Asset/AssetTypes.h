#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Log.h"

namespace Hanabi
{
	enum class AssetType : uint16_t
	{
		None = 0,
		Scene,
		Texture2D,
		Material,
	};

	namespace Utils
	{
		inline std::string_view AssetTypeToString(AssetType type)
		{
			switch (type)
			{
			case AssetType::None:      return "AssetType::None";
			case AssetType::Scene:     return "AssetType::Scene";
			case AssetType::Texture2D: return "AssetType::Texture2D";
			}

			return "AssetType::<Invalid>";
		}

		inline AssetType AssetTypeFromString(std::string_view assetType)
		{
			if (assetType == "AssetType::None")      return AssetType::None;
			if (assetType == "AssetType::Scene")     return AssetType::Scene;
			if (assetType == "AssetType::Texture2D") return AssetType::Texture2D;

			return AssetType::None;
		}
	}
}