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
		Buffer imageBuffer;
		std::string pathString = path.string();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		if (stbi_is_hdr(pathString.c_str()))
		{
			imageBuffer.Data = (uint8_t*)stbi_loadf(pathString.c_str(), &width, &height, &channels, 4);
			imageBuffer.Size = width * height * 4 * sizeof(float);
			spec.Format = ImageFormat::RGBA32F;
		}
		else
		{
			imageBuffer.Data = stbi_load(pathString.c_str(), &width, &height, &channels, 4);
			imageBuffer.Size = width * height * 4;
			spec.Format = ImageFormat::RGBA8;
		}

		if (!imageBuffer)
			return {};

		spec.Width = width;
		spec.Height = height;
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