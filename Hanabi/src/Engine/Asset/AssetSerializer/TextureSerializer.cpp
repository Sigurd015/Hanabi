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

	Buffer TextureSerializer::ToBufferFromFile(const std::filesystem::path& path, TextureSpecification& spec)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		Buffer imageBuffer;
		std::string pathStr = path.string();
		imageBuffer.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 4);

		if (!imageBuffer)
		{
			HNB_CORE_ERROR("TextureSerializer::LoadTexture2D - Could not load texture from filepath: {}", path.string());
			return {};
		}

		imageBuffer.Size = width * height * channels;

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

		return imageBuffer;
	}

	Ref<Texture2D> TextureSerializer::LoadTexture2D(const std::filesystem::path& path)
	{
		TextureSpecification spec;
		Buffer data = ToBufferFromFile(path, spec);

		if (data)
		{
			Ref<Texture2D> texture = Texture2D::Create(spec, data);
			data.Release();
			return texture;
		}

		return nullptr;
	}
}