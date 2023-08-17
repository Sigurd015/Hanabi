#pragma once
#include "Engine/Core/UUID.h"
#include "AssetTypes.h"

namespace Hanabi
{
	using AssetHandle = UUID;

	class Asset
	{
	public:
		AssetHandle Handle; // Generate handle

		virtual AssetType GetAssetType() const = 0;
	};
}