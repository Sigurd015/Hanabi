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
		Material(const Ref<Shader>& shader = nullptr);
		virtual ~Material();
		void Bind() const;

		void SetTexture(const Ref<Texture2D>& texture, uint32_t index);

		Ref<Shader> GetShader() { return m_Shader; }

		// Phong Lighting Model for now
		enum class TextureType
		{
			Diffuse = 0,
			Specular = 1,
		};
		/// <summary>
		/// Notice: For Phong Lighting Model, 0 is Diffuse, 1 is Specular
		/// </summary>
		Ref<Texture2D> GetTexture(TextureType type);

		static Ref<Material> Create(const Ref<Shader>& shader);
	private:
		void BindTextures() const;

		Ref<Shader> m_Shader;
		std::unordered_map<uint32_t, Ref<Texture2D>> m_Textures;
	};
}