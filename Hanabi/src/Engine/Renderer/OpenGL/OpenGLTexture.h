#pragma once
#include "Engine/Renderer/Texture.h"
#include <glad/glad.h>

namespace Hanabi
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(uint32_t width, uint32_t height);
		~OpenGLTexture2D();
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		uint32_t GetRendererID() const override { return m_RendererID; }
		virtual bool IsLoaded() const override { return m_IsLoaded; }
		void Bind(uint32_t slot = 0) const override;
		void SetData(void* data, uint32_t size) override;
		bool operator==(const Texture& other) const override;
	private:
		std::string m_Path;
		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}