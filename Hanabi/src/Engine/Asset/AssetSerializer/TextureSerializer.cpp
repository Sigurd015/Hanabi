#include "hnbpch.h"
#include "AssetSerializer.h"
#include "Engine/Project/Project.h"

#include <stb_image.h>
#include <fstream>

namespace Hanabi
{
	/// <summary>
	/// Notice: This function is only used for loading textures, so we generate mipmaps for all textures by default
	/// Environment maps, Editor icons, etc. should be loaded using the LoadTexture2D function
	/// Which allows us to specify whether we want to generate mipmaps or not
	/// </summary>
	bool TextureSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		TextureSpecification spec = {};
		spec.GenerateMips = true;
		asset = LoadTexture2D(Project::GetEditorAssetManager()->GetFileSystemPath(metadata), spec);
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
		if (stbi_is_hdr(pathString.c_str()))
		{
			imageBuffer.Data = (uint8_t*)stbi_loadf(pathString.c_str(), &width, &height, &channels, STBI_rgb_alpha);
			imageBuffer.Size = width * height * 4 * sizeof(float);
			spec.Format = ImageFormat::RGBA32F;
			spec.GenerateMips = false;
		}
		else
		{
			stbi_set_flip_vertically_on_load(1);
			imageBuffer.Data = stbi_load(pathString.c_str(), &width, &height, &channels, STBI_rgb_alpha);
			imageBuffer.Size = width * height * 4;
			spec.Format = ImageFormat::SRGB;
			spec.GenerateMips = true;
		}

		if (!imageBuffer)
			return {};

		spec.Width = width;
		spec.Height = height;
		return imageBuffer;
	}

	Ref<Texture2D> TextureSerializer::LoadTexture2D(const std::filesystem::path& path, TextureSpecification& spec)
	{
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