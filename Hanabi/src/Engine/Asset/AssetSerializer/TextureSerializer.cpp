#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Engine/Project/Project.h"

#include <stb_image.h>
#include <fstream>

namespace Hanabi
{
	bool TextureSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		asset = LoadTexture2D(Project::GetEditorAssetManager()->GetFileSystemPath(metadata));
		if (asset)
		{
			asset->Handle = metadata.Handle;
			return true;
		}

		return false;
	}

	Ref<Texture2D> TextureSerializer::LoadTexture2D(const std::filesystem::path& path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		Buffer data;
		std::string pathStr = path.string();
		data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 4);

		if (data.Data == nullptr)
		{
			HNB_CORE_ERROR("TextureSerializer::LoadTexture2D - Could not load texture from filepath: {}", path.string());
			return nullptr;
		}

		data.Size = width * height * channels;

		TextureSpecification spec;
		spec.Width = width;
		spec.Height = height;
		switch (channels)
		{
		case 3:
			spec.Format = ImageFormat::RGB8;
			break;
		case 4:
			spec.Format = ImageFormat::RGBA8;
			break;
		}

		Ref<Texture2D> texture = Texture2D::Create(spec, data);
		data.Release();
		return texture;
	}
}