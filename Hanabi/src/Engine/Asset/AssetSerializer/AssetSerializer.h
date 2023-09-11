#pragma once
#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Renderer/MaterialAsset.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/EnvMapAsset.h"

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

		static void SerializeToYAML(const std::filesystem::path& path, const Ref<Scene>& scene);
		static Ref<Scene> DeserializeFromYAML(const std::filesystem::path& path);
	};

	class MeshSourceSerializer : public AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override {}
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;

	private:
		static Ref<MeshSource> LoadMeshSource(const std::filesystem::path& path);
	};

	class EnvMapAssetSerializer : public AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;

		// Use for creating new env map assets
		static void SerializeToYAML(const std::filesystem::path& path);
	private:
		static std::string SerializeToYAML(Ref<EnvMapAsset> envMapAsset);
		static bool DeserializeFromYAML(const std::string& yamlString, Ref<EnvMapAsset>& targetEnvMapAsset);
	};

	class MaterialAssetSerializer : public AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;

		// Use for creating new material assets
		static void SerializeToYAML(const std::filesystem::path& path);
	private:
		static std::string SerializeToYAML(Ref<MaterialAsset> materialAsset);
		static bool DeserializeFromYAML(const std::string& yamlString, Ref<MaterialAsset>& targetMaterialAsset);
	};

	class TextureSerializer : public AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override {}
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const override;

		// Reads file directly from filesystem
		// (i.e. path has to be relative / absolute to working directory)
		static Buffer LoadTextureData(const std::filesystem::path& path, TextureSpecification& spec);
		static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& path);
	};
}