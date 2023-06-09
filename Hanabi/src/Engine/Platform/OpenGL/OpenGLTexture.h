#pragma once
#include "Engine/Renderer/Texture.h"

#include <glad/glad.h>

namespace Hanabi
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(const TextureSpecification& specification);
		~OpenGLTexture2D();
		const TextureSpecification& GetSpecification() const override { return m_Specification; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		void* GetRendererID() const override { return (void*)m_RendererID; }
		virtual bool IsLoaded() const override { return m_IsLoaded; }
		void Bind(uint32_t slot = 0) const override;
		void SetData(void* data, uint32_t size) override;
		virtual const std::string& GetPath() const override { return m_Path; }
		bool operator==(const Texture& other) const override;
	private:
		TextureSpecification m_Specification;
		std::string m_Path;
		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}