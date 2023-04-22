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

		HNB_INFO("Texcoords 0:{0},{1}", m_TexCoords[0].x, m_TexCoords[0].y);
		HNB_INFO("Texcoords 1:{0},{1}", m_TexCoords[1].x, m_TexCoords[1].y);
		HNB_INFO("Texcoords 2:{0},{1}", m_TexCoords[2].x, m_TexCoords[2].y);
		HNB_INFO("Texcoords 3:{0},{1}", m_TexCoords[3].x, m_TexCoords[3].y);
	}
	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(Ref<Texture2D> texture, glm::vec2& coords, glm::vec2& spriteSize)
	{
		glm::vec2 min((coords.x * spriteSize.x) / texture->GetWidth(), (coords.y * spriteSize.y) / texture->GetHeight());
		glm::vec2 max(((coords.x + 1) * spriteSize.x) / texture->GetWidth(), ((coords.y + 1) * spriteSize.y) / texture->GetHeight());
		return CreateRef<SubTexture2D>(texture, min, max);
	}
}