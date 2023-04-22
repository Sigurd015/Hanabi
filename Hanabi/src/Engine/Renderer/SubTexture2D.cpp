#include "hnbpch.h"
#include "Engine/Renderer/SubTexture2D.h"

namespace Hanabi
{
	SubTexture2D::SubTexture2D(Ref<Texture2D> texture, glm::vec2& min, glm::vec2& max) :m_Texture(texture)
	{
		m_TexCoords[0] = { min.x,min.y };
		m_TexCoords[1] = { max.x,min.y };
		m_TexCoords[2] = { max.x,max.y };
		m_TexCoords[3] = { min.x,max.y };
	}
	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(Ref<Texture2D> texture, glm::vec2& coords, glm::vec2& cellSize, glm::vec2& spriteSize)
	{
		glm::vec2 min((coords.x * cellSize.x) / texture->GetWidth(), (coords.y * cellSize.y) / texture->GetHeight());
		glm::vec2 max(((coords.x + spriteSize.x) * cellSize.x) / texture->GetWidth(), ((coords.y + spriteSize.y) * cellSize.y) / texture->GetHeight());
		return CreateRef<SubTexture2D>(texture, min, max);
	}
}