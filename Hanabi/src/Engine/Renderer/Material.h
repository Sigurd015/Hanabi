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
		virtual ~Material();
		void Bind() const;

		void SetTexture(const Ref<Texture2D>& texture, uint32_t index);
		Ref<Texture2D> GetTexture(uint32_t index);

		Ref<Shader> GetShader() { return m_Shader; }

		static Ref<Material> Create(const Ref<Shader>& shader);
		static Ref<Material> Copy(const Ref<Material>& other);
	private:
		void BindTextures() const;

		Ref<Shader> m_Shader;
		std::unordered_map<uint32_t, Ref<Texture2D>> m_Textures;
	};
}