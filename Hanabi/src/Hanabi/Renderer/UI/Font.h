#pragma once
#include "Hanabi/Core/Base.h"
#include "Hanabi/Renderer/Texture.h"
#include "MSDFData.h"

#include <filesystem>
namespace Hanabi
{
	class Font
	{
	public:
		Font(const std::filesystem::path& path);
		~Font();
		Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }
		const MSDFData* GetMSDFData() const { return m_Data; }
		static Ref<Font> GetDefault();
	private:
		MSDFData* m_Data;
		Ref<Texture2D> m_AtlasTexture;
	};
}