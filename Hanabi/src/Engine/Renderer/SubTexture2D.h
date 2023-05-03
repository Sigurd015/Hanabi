#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Renderer/Texture.h"
#include <glm/glm.hpp>

namespace Hanabi
{
	class SubTexture2D
	{
	public:
		SubTexture2D(Ref<Texture2D> texture, glm::vec2& min, glm::vec2& max);
		const Ref<Texture2D> GetTexture() { return m_Texture; }
		const glm::vec2* GetTexCoords() { return m_TexCoords; }
		static Ref<SubTexture2D> CreateFromCoords(Ref<Texture2D> texture, 
			glm::vec2& coords, glm::vec2& cellSize, glm::vec2& spriteSize = glm::vec2(1.0f));
	private:
		Ref<Texture2D> m_Texture;
		glm::vec2 m_TexCoords[4];
	};
}