#include "hnbpch.h"
#include "EnvMapAsset.h"

namespace Hanabi
{
	Ref<TextureCube> EnvMapAsset::GetRadianceMap() const
	{
		return Ref<TextureCube>();
	}

	Ref<TextureCube> EnvMapAsset::GetIrradianceMap() const
	{
		return Ref<TextureCube>();
	}

	void EnvMapAsset::Invalidate()
	{

	}
}