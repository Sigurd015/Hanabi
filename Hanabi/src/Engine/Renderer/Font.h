#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Renderer/Texture.h"

#include <filesystem>
namespace Hanabi
{
	struct MSDFData;

	class Font
	{
	public:
		Font(const std::filesystem::path& path);
		~Font();
		Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }
	private:
		MSDFData* m_Data;
		Ref<Texture2D> m_AtlasTexture;
	};
}