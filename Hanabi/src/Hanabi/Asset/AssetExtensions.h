#pragma once
#include "AssetTypes.h"

namespace Hanabi
{
	inline static std::unordered_map<std::string, AssetType> s_AssetExtensionMap =
	{
		{ ".hscene", AssetType::Scene },

		// Textures
		{ ".png", AssetType::Texture2D },
		{ ".jpg", AssetType::Texture2D },
		{ ".jpeg", AssetType::Texture2D },
		{ ".tga", AssetType::Texture2D },
		{ ".hdr", AssetType::EnvMap },

		// Materials
		{ ".hmat", AssetType::Material },

		// MeshSource
		{ ".fbx", AssetType::MeshSource },
		{ ".obj", AssetType::MeshSource },
		{ ".gltf", AssetType::MeshSource },
	};
}