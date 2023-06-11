#pragma once
#include "Engine/Renderer/VertexBuffer.h"

namespace Hanabi
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const void* vertices, uint32_t size);
		~OpenGLVertexBuffer();
		void Bind() const override;
		void SetData(const void* data, uint32_t size) override;
		const VertexBufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const VertexBufferLayout& layout) override { m_Layout = layout; }
	private:
		uint32_t m_RendererID;
		VertexBufferLayout m_Layout;
	};
}