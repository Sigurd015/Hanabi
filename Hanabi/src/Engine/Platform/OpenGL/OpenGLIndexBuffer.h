#pragma once
#include "Engine/Renderer/IndexBuffer.h"

namespace Hanabi
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(void* indices, uint32_t count);
		~OpenGLIndexBuffer();
		void Bind() const override;
		uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};
}