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

	Buffer TextureSerializer::LoadTextureData(const std::filesystem::path& path, ImageFormat& outFormat, uint32_t& outWidth, uint32_t& outHeight)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		Buffer data;
		std::string pathStr = path.string();
		data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 4);

		if (data.Data == nullptr)
		{
			HNB_CORE_ERROR("TextureSerializer::LoadTexture2D - Could not load texture from filepath: {}", path.string());
			return 0;
		}

		data.Size = width * height * channels;

		outWidth = width;
		outHeight = height;
		switch (channels)
		{
		case 3:
			outFormat = ImageFormat::RGB8;
			break;
		case 4:
			outFormat = ImageFormat::RGBA8;
			break;
		}

		return data;
	}

	Ref<Texture2D> TextureSerializer::LoadTexture2D(const std::filesystem::path& path)
	{
		TextureSpecification spec;
		Buffer data = LoadTextureData(path, spec.Format, spec.Width, spec.Height);

		if (data)
		{
			Ref<Texture2D> texture = Texture2D::Create(spec, data);
			data.Release();
			return texture;
		}

		return nullptr;
	}
}