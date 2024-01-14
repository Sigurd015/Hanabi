#ifdef HNB_ENABLE_D3D
#pragma once
#include "Hanabi/Renderer/VertexBuffer.h"
#include "Hanabi/Platform/D3D/DXCommon.h"

#include <d3d11.h>

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
		ComPtr<ID3D11Buffer> m_VertexBuffer;
	};
}
#endif