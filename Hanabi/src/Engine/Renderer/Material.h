#pragma once
#include "Shader.h"
#include "Texture.h"

#include <iostream>
#include <string>

namespace Hanabi
{
	class Material
	{
	public:
		Material(const Ref<Shader>& shader);
		~Material() = default;
		void Bind() const;

		template <typename T>
		void SetTexture(const std::string& name, const Ref<T>& texture)
		{
			static_assert(std::is_base_of<Texture, T>::value, "Material::SetTexture only works for types derived from Texture");

			SetTextureInternal(name, texture);
		}

		template <typename T>
		Ref<T> GetTexture(const std::string& name)
		{
			static_assert(std::is_base_of<Texture, T>::value, "Material::GetTexture only works for types derived from Texture");

			return std::static_pointer_cast<T>(GetTextureInternal(name));
		}

		Ref<Shader> GetShader() { return m_Shader; }

		static Ref<Material> Create(const Ref<Shader>& shader);
		static Ref<Material> Copy(const Ref<Material>& other);
	private:
		void SetTextureInternal(const std::string& name, const Ref<Texture>& texture);
		Ref<Texture> GetTextureInternal(const std::string& name);
		void BindTextures() const;

		Ref<Shader> m_Shader;
		std::unordered_map<std::string, Ref<Texture>> m_Textures;
	};
}