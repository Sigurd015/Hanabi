#pragma once
#include "Shader.h"
#include "Texture.h"

#include <iostream>
#include <string>

#define MAX_TEXTURES 3

namespace Hanabi
{
	class Material
	{
	public:
		Material(const Ref<Shader>& shader);
		~Material() = default;
		void Bind() const;

		template <typename T>
		void SetTexture(const Ref<T>& texture, uint32_t index = 0)
		{
			static_assert(std::is_base_of<Texture, T>::value, "Material::SetTexture only works for types derived from Texture");

			SetTextureInternal(texture, index);
		}

		template <typename T>
		Ref<T> GetTexture(uint32_t index = 0)
		{
			static_assert(std::is_base_of<Texture, T>::value, "Material::GetTexture only works for types derived from Texture");

			return std::static_pointer_cast<T>(GetTextureInternal(index));
		}

		Ref<Shader> GetShader() { return m_Shader; }

		static Ref<Material> Create(const Ref<Shader>& shader);
		static Ref<Material> Copy(const Ref<Material>& other);
	private:
		void SetTextureInternal(const Ref<Texture>& texture, uint32_t index);
		Ref<Texture> GetTextureInternal(uint32_t index);
		void BindTextures() const;

		Ref<Shader> m_Shader;
		std::array<Ref<Texture>, MAX_TEXTURES> m_Textures;
	};
}