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
		Mesh,
		MeshSource,
		EnvMap,
	};

	namespace Utils
	{
		inline std::string AssetTypeToString(AssetType type)
		{
			switch (type)
			{
			case AssetType::None:      return "AssetType::None";
			case AssetType::Scene:     return "AssetType::Scene";
			case AssetType::Texture2D: return "AssetType::Texture2D";
			case AssetType::Material: return "AssetType::Material";
			case AssetType::Mesh: return "AssetType::Mesh";
			case AssetType::MeshSource: return "AssetType::MeshSource";
			case AssetType::EnvMap: return "AssetType::EnvMap";
			}

			return "AssetType::<Invalid>";
		}

		inline AssetType AssetTypeFromString(const std::string& assetType)
		{
			if (assetType == "AssetType::None")      return AssetType::None;
			if (assetType == "AssetType::Scene")     return AssetType::Scene;
			if (assetType == "AssetType::Texture2D") return AssetType::Texture2D;
			if (assetType == "AssetType::Material") return AssetType::Material;
			if (assetType == "AssetType::Mesh") return AssetType::Mesh;
			if (assetType == "AssetType::MeshSource") return AssetType::MeshSource;
			if (assetType == "AssetType::EnvMap") return AssetType::EnvMap;

			return AssetType::None;
		}
	}
}