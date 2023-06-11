#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/VertexBuffer.h"

#include <d3d11.h>
#include <wrl.h>

namespace Hanabi
{
	class DX11VertexBuffer : public VertexBuffer
	{
	public:
		DX11VertexBuffer(uint32_t size);
		DX11VertexBuffer(const void* vertices, uint32_t size);
		~DX11VertexBuffer();
		void Bind() const override;
		void SetData(const void* data, uint32_t size) override;
		const VertexBufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const VertexBufferLayout& layout)override { m_Layout = layout;	m_Stride = m_Layout.GetStride(); }
	private:
		uint32_t m_Stride = 0;
		VertexBufferLayout m_Layout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	};
}
#endif