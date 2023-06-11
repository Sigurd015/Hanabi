#pragma once
#include "Shader.h"
#include "Texture.h"

#include <unordered_set>
#include <iostream>
#include <string>

namespace Hanabi
{
	class Material
	{
	public:
		Material(const Ref<Shader>& shader);
		virtual ~Material();
		void Bind() const;
		void SetTexture(const Ref<Texture>& texture)
		{
			if (m_Textures.size() <= m_TextureSlotIndex)
				m_Textures.resize((size_t)m_TextureSlotIndex + 1);
			m_Textures[m_TextureSlotIndex] = texture;
			m_TextureSlotIndex++;
		}

		void SetTexture(const Ref<Texture2D>& texture)
		{
			SetTexture((const Ref<Texture>&)texture);
		}

		static Ref<Material> Create(const Ref<Shader>& shader);
	private:
		void BindTextures() const;

		Ref<Shader> m_Shader;
		std::vector<Ref<Texture>> m_Textures;

		uint32_t m_TextureSlotIndex = 0;
	};
}