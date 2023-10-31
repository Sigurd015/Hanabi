#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Engine/Project/Project.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Hanabi
{
	bool EnvMapAssetSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		TextureSpecification spec = {};
		spec.GenerateMips = false;
		Ref<Texture2D> equirectangularMap = TextureSerializer::LoadTexture2D(Project::GetEditorAssetManager()->GetFileSystemPath(metadata), spec);
		auto [radiance, irradiance] = Renderer::CreateEnvironmentMap(equirectangularMap);
		asset = CreateRef<EnvMapAsset>(radiance, irradiance);
		asset->Handle = metadata.Handle;

		// Debug
		//asset = TextureSerializer::LoadTexture2D(Project::GetEditorAssetManager()->GetFileSystemPath(metadata), spec);
		//asset->Handle = metadata.Handle;
		return true;
	}
}