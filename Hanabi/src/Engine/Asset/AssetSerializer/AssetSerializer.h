#pragma once
#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Renderer/MaterialAsset.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Scene/Scene.h"

namespace Hanabi
{
	class AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const = 0;
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const = 0;
	};

	class SceneSerializer : public AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;

		static Ref<Scene> LoadScene(const std::filesystem::path& path);
	};

	class MeshSourceSerializer : public AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override {}
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;

	private:
		static Ref<MeshSource> LoadMeshSource(const std::filesystem::path& path);
	};

	class MaterialAssetSerializer : public AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;

	private:
		std::string SerializeToYAML(Ref<MaterialAsset> materialAsset) const;
		bool DeserializeFromYAML(const std::string& yamlString, Ref<MaterialAsset>& targetMaterialAsset) const;
	};

	class TextureSerializer : public AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override {}
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;

		// Reads file directly from filesystem
		// (i.e. path has to be relative / absolute to working directory)
		static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& path);
	};
}