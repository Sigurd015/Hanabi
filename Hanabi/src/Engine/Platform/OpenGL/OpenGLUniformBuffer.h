#pragma once
#include "Engine/Renderer/ConstantBuffer.h"

namespace Hanabi
{
	class OpenGLUniformBuffer : public ConstantBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();
		void Bind() const override;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_BindingID;
	};
}