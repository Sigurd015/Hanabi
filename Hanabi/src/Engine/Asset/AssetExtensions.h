#pragma once
#include "AssetTypes.h"

namespace Hanabi
{
	inline static std::unordered_map<std::string, AssetType> s_AssetExtensionMap =
	{
		{ ".scene", AssetType::Scene },

		// Textures
		{ ".png", AssetType::Texture2D },
		{ ".jpg", AssetType::Texture2D },
		{ ".jpeg", AssetType::Texture2D },

		//Materials
		{ ".mat", AssetType::Material },
	};
}