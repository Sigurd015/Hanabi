#include "hnbpch.h"
#include "TextureImporter.h"
#include "Engine/Project/Project.h"

#include <stb_image.h>

namespace Hanabi
{
	Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadTexture2D(Project::GetAssetDirectory() / metadata.FilePath);
	}

	Ref<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		Buffer data;
		std::string pathStr = path.string();
		data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 0);

		if (data.Data == nullptr)
		{
			HNB_CORE_ERROR("TextureImporter::ImportTexture2D - Could not load texture from filepath: {}", path.string());
			return nullptr;
		}

		// TODO: think about this
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