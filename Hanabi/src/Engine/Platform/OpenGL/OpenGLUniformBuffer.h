#pragma once
#include "Engine/Renderer/ConstantBuffer.h"

namespace Hanabi
{
	class OpenGLUniformBuffer : public ConstantBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size);
		virtual ~OpenGLUniformBuffer();
		void Bind(uint32_t slot) const override;
		virtual void SetData(const void* data, uint32_t offset = 0) override;
	private:
		uint32_t m_DataSize;
		uint32_t m_RendererID = 0;
	};
}